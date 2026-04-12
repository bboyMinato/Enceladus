#include "RenderSystem.h"

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
	RenderTexture(texture, nullptr, dstRect);
}

void RenderSystem::RenderTexture(SDL_Texture* texture, const SDL_Rect* srcRect, const SDL_Rect* dstRect) const
{
	if (!m_renderer || !texture)
	{
		return;
	}

	SDL_RenderCopy(m_renderer, texture, srcRect, dstRect);
}

void RenderSystem::RenderTexture(const std::string& textureName, const SDL_Rect* dstRect) const
{
	RenderTexture(textureName, nullptr, dstRect);
}

void RenderSystem::RenderTexture(const std::string& textureName, const SDL_Rect* srcRect, const SDL_Rect* dstRect) const
{
	RenderTexture(m_textureManager->GetTexture(textureName), srcRect, dstRect);
}