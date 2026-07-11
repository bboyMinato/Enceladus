#include "RenderSystem.h"
#include "../ECS/SpriteComponent.h"
#include "../ECS/TransformComponent.h"
#include "Core/Managers/DialogManager.h"
#include "Core/Managers/TextManager.h"

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

void RenderSystem::RenderDialog(const Dialogue* dialogue, int windowWidth, int windowHeight, TextManager& textManager) const
{
	if (!m_renderer || !dialogue)
	{
		return;
	}

	const auto& entries = dialogue->GetEntries();
	if (entries.empty())
	{
		return;
	}

	constexpr int padding = 16;
	constexpr int dialogHeight = 96;

	const int x = padding;
	const int y = windowHeight - padding - dialogHeight;
	const int w = windowWidth - 2 * padding;
	const int h = dialogHeight;

	SDL_Rect dialogRect{ x, y, w, h };

	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(m_renderer, &dialogRect);
	SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawRect(m_renderer, &dialogRect);

	const std::string textName = "dialogueText";
	const std::string text = entries.front().GetSpeech();

	if (!textManager.LoadText(textName, "menuFont", text, { 255, 255, 255, SDL_ALPHA_OPAQUE }))
	{
		return;
	}

	SDL_Texture* textTexture = textManager.GetText(textName);
	if (!textTexture)
	{
		return;
	}

	int textWidth = 0;
	int textHeight = 0;
	textManager.GetTextSize(textName, textWidth, textHeight);

	SDL_Rect textRect
	{
		x + (w - textWidth) / 2,
		y + (h - textHeight) / 2,
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

	SDL_SetTextureColorMod(textTexture, 0, 0, 0);
	SDL_SetTextureAlphaMod(textTexture, 170);
	SDL_RenderCopy(m_renderer, textTexture, nullptr, &textShadowRect);

	SDL_SetTextureColorMod(textTexture, 235, 225, 210);
	SDL_SetTextureAlphaMod(textTexture, 255);
	SDL_RenderCopy(m_renderer, textTexture, nullptr, &textRect);

	SDL_SetTextureColorMod(textTexture, 255, 255, 255);
	SDL_SetTextureAlphaMod(textTexture, 255);
}
