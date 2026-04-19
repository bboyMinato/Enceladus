#include "RenderSystem.h"
#include "../ECS/TransformComponent.h"
#include "../ECS/SpriteComponent.h"

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