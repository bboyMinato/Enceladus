#include "RenderSystem.h"
#include "../ECS/SpriteComponent.h"
#include "../ECS/TransformComponent.h"
#include "../Managers/TextManager.h"
#include <algorithm>
#include <sstream>
#include <vector>

namespace
{
	std::vector<std::string> WrapTextToLines(TTF_Font* font, const std::string& text, int maxWidth)
	{
		std::vector<std::string> lines;

		if (!font)
		{
			return lines;
		}

		const auto appendParagraph = [&](const std::string& paragraph)
		{
			if (paragraph.empty())
			{
				lines.emplace_back();
				return;
			}

			std::istringstream stream(paragraph);
			std::string word;
			std::string currentLine;

			while (stream >> word)
			{
				const std::string candidate = currentLine.empty() ? word : currentLine + " " + word;
				int candidateWidth = 0;
				int candidateHeight = 0;
				TTF_SizeUTF8(font, candidate.c_str(), &candidateWidth, &candidateHeight);

				if (maxWidth > 0 && candidateWidth > maxWidth && !currentLine.empty())
				{
					lines.push_back(currentLine);
					currentLine = word;
					continue;
				}

				currentLine = candidate;
			}

			if (!currentLine.empty())
			{
				lines.push_back(currentLine);
			}
		};

		std::size_t start = 0;
		while (start <= text.size())
		{
			const std::size_t end = text.find('\n', start);
			if (end == std::string::npos)
			{
				appendParagraph(text.substr(start));
				break;
			}

			appendParagraph(text.substr(start, end - start));
			start = end + 1;
		}

		return lines;
	}
}

bool RenderSystem::Init(SDL_Renderer* renderer, TextureManager* textureManager)
{
    if (!renderer || !textureManager)
    {
        return false;
	}

	m_renderer = renderer;
	m_textureManager = textureManager;

	return true;
}

void RenderSystem::Shutdown()
{
	m_renderer = nullptr;
	m_textureManager = nullptr;
}

void RenderSystem::GetOutputSize(int& width, int& height) const
{
	if (!m_renderer)
	{
		width = 0;
		height = 0;
		return;
	}

	SDL_GetRendererOutputSize(m_renderer, &width, &height);
}

void RenderSystem::RenderTexture(SDL_Texture* texture, const SDL_Rect* dstRect) const
{
	RenderTexture(texture, nullptr, dstRect, SDL_FLIP_NONE);
}

void RenderSystem::RenderTexture(SDL_Texture* texture, const SDL_Rect* srcRect, const SDL_Rect* dstRect, SDL_RendererFlip flip) const
{
	if (!m_renderer || !texture)
	{
		return;
	}

	SDL_RenderCopyEx(m_renderer, texture, srcRect, dstRect, 0.0, nullptr, flip);
}

void RenderSystem::RenderTexture(const std::string& textureName, const SDL_Rect* dstRect) const
{
	RenderTexture(textureName, nullptr, dstRect);
}

void RenderSystem::RenderTexture(const std::string& textureName, const SDL_Rect* srcRect, const SDL_Rect* dstRect, SDL_RendererFlip flip) const
{
	RenderTexture(m_textureManager->GetTexture(textureName), srcRect, dstRect, flip);
}

void RenderSystem::RenderEntites(Registry& registry, const CameraComponent& camera) const
{
	registry.ForEach<TransformComponent, SpriteComponent>(
		[&](Entity entity, const TransformComponent& transform, const SpriteComponent& sprite)
		{
			if (!sprite.m_isVisible)
			{
				return;
			}

			SDL_Rect dstRect
			{
				static_cast<int>(transform.x) - camera.m_viewport.x,
				static_cast<int>(transform.y) - camera.m_viewport.y,
				sprite.m_width,
				sprite.m_height
			};

			const SDL_Rect* srcRect = sprite.m_hasSourceRect ? &sprite.m_sourceRect : nullptr;
			RenderTexture(sprite.m_textureName, srcRect, &dstRect, sprite.m_flip);
		}
	);		
}

void RenderSystem::RenderDialogue(const DialogueRuntimeState& dialogueState, int windowWidth, int windowHeight, TextManager& textManager) const
{
	if (!m_renderer || !dialogueState.HasDialogue())
	{
		return;
	}

	constexpr int paddingTopBottom = 16;
    constexpr int paddingLeftRight = 256;

	const int x = paddingLeftRight;
	const int w = windowWidth - 2 * paddingLeftRight;

	TTF_Font* font = textManager.GetFont("menuFont");
	if (!font)
	{
		return;
	}

	constexpr std::size_t visibleLineCount = 2;
	const int lineSkip = TTF_FontLineSkip(font);
	const int dialogHeight = paddingTopBottom * 2 + static_cast<int>(visibleLineCount) * lineSkip;
	const int y = windowHeight - paddingTopBottom - dialogHeight;
	const int h = dialogHeight;

	SDL_Rect dialogRect{ x, y, w, h };

	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(m_renderer, &dialogRect);
	SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawRect(m_renderer, &dialogRect);

	const std::string text = dialogueState.GetVisibleText();
	if (text.empty())
	{
		return;
	}

	const int wrapWidth = w > 32 ? w - 32 : 0;
	const std::vector<std::string> wrappedLines = WrapTextToLines(font, text, wrapWidth);
	if (wrappedLines.empty())
	{
		return;
	}

	const std::size_t linesToDraw = std::min(visibleLineCount, wrappedLines.size());
	const std::size_t firstLine = wrappedLines.size() > linesToDraw ? wrappedLines.size() - linesToDraw : 0;
	const int firstLineY = y + paddingTopBottom;

	for (std::size_t lineIndex = 0; lineIndex < linesToDraw; ++lineIndex)
	{
		const std::string& line = wrappedLines[firstLine + lineIndex];
		if (line.empty())
		{
			continue;
		}

		const std::string textName = "dialogueLine" + std::to_string(lineIndex);
		if (!textManager.LoadText(textName, "menuFont", line, { 255, 255, 255, SDL_ALPHA_OPAQUE }))
		{
			continue;
		}

		SDL_Texture* textTexture = textManager.GetText(textName);
		if (!textTexture)
		{
			continue;
		}

		int textWidth = 0;
		int textHeight = 0;
		textManager.GetTextSize(textName, textWidth, textHeight);

		const int textX = x + (w - textWidth) / 2;
		const int textY = firstLineY + static_cast<int>(lineIndex) * lineSkip;

		SDL_Rect textRect
		{
			textX,
			textY,
			textWidth,
			textHeight
		};

		SDL_Rect textShadowRect
		{
			textRect.x + 3,
			textRect.y + 3,
			textRect.w,
			textRect.h
		};

		SDL_SetTextureColorMod(textTexture, 47, 47, 0);
		SDL_SetTextureAlphaMod(textTexture, 170);
		SDL_RenderCopy(m_renderer, textTexture, nullptr, &textShadowRect);

		SDL_SetTextureColorMod(textTexture, 235, 225, 210);
		SDL_SetTextureAlphaMod(textTexture, 255);
		SDL_RenderCopy(m_renderer, textTexture, nullptr, &textRect);

		SDL_SetTextureColorMod(textTexture, 255, 255, 255);
		SDL_SetTextureAlphaMod(textTexture, 255);
	}
}
