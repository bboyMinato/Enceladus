#include "RenderSystem.h"
#include "../ECS/SpriteComponent.h"
#include "../ECS/TransformComponent.h"
#include "../Managers/TextManager.h"
#include <algorithm>
#include <sstream>
#include <vector>
#include "../ECS/ColliderComponent.h"

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

	const DialogueEntry* entry = dialogueState.GetCurrentEntry();
	const DialogueNode* node = dialogueState.GetCurrentNode();
	TTF_Font* font = textManager.GetFont("dialogueFont");

	if (!entry || !node || !font)
	{
		return;
	}

	constexpr int horizontalMargin = 80;
	constexpr int padding = 24;
	constexpr int optionSpacing = 8;

	const int dialogWidth = windowWidth - horizontalMargin * 2;
	const int lineHeight = TTF_FontLineSkip(font);
	const int optionCount = dialogueState.m_showingOptions
		? static_cast<int>(node->m_options.size())
		: 0;
	const int dialogHeight = padding * 2 + lineHeight * (3 + optionCount) +
		optionSpacing * optionCount;

	const SDL_Rect dialogRect
	{
		horizontalMargin,
		windowHeight - dialogHeight - 48,
		dialogWidth,
		dialogHeight
	};

	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 220);
	SDL_RenderFillRect(m_renderer, &dialogRect);

	SDL_SetRenderDrawColor(m_renderer, 235, 225, 210, 255);
	SDL_RenderDrawRect(m_renderer, &dialogRect);

	const auto renderText = [&](const std::string& name,
		const std::string& value,
		SDL_Color color,
		int x,
		int y,
		int wrapWidth = 0)
		{
			if (!textManager.LoadText(name, "dialogueFont", value, color, wrapWidth))
			{
				return;
			}

			SDL_Texture* texture = textManager.GetText(name);
			if (!texture)
			{
				return;
			}

			int width = 0;
			int height = 0;
			textManager.GetTextSize(name, width, height);

			const SDL_Rect destination{ x, y, width, height };
			SDL_RenderCopy(m_renderer, texture, nullptr, &destination);
		};

	const int textX = dialogRect.x + padding;
	int textY = dialogRect.y + padding;

	renderText(
		"dialogueSpeaker",
		entry->m_name,
		{ 255, 215, 120, 255 },
		textX,
		textY);

	textY += lineHeight + 8;

	renderText(
		"dialogueSpeech",
		entry->m_speech,
		{ 235, 225, 210, 255 },
		textX,
		textY,
		dialogRect.w - padding * 2);

	if (!dialogueState.m_showingOptions)
	{
		return;
	}

	textY += lineHeight * 2 + 8;

	for (std::size_t index = 0; index < node->m_options.size(); ++index)
	{
		const bool isSelected = index == dialogueState.m_selectedOptionIndex;
		const SDL_Color color = isSelected
			? SDL_Color{ 255, 215, 120, 255 }
			: SDL_Color{ 235, 225, 210, 255 };

		const std::string prefix = isSelected ? "> " : "  ";
		renderText(
			"dialogueOption" + std::to_string(index),
			prefix + node->m_options[index].m_text,
			color,
			textX,
			textY);

		textY += lineHeight + optionSpacing;
	}
}

void RenderSystem::RenderDebugCollider(Registry& registry, const CameraComponent& camera, int selectedEntityId, bool showDebug) const
{
	if (!showDebug)
	{
		return;
	}

	for (auto& entity : registry.GetEntitiesWithComponents<ColliderComponent, TransformComponent>())
	{
		const auto& collider = entity.Get<ColliderComponent>();
		const auto& transform = entity.Get<TransformComponent>();

		SDL_Rect colliderRect
		{
			static_cast<int>(transform->x + collider->m_offsetX - camera.m_viewport.x),
			static_cast<int>(transform->y + collider->m_offsetY - camera.m_viewport.y),
			collider->m_width,
			collider->m_height
		};

		SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);

		if (selectedEntityId == entity.GetId())
		{
			SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);
		}

		SDL_RenderDrawRect(m_renderer, &colliderRect);
	}
}
