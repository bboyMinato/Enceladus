#pragma once
#include "../Managers/TextureManager.h"
#include <SDL2/SDL.h>

class RenderSystem
{
public:
	bool Init(SDL_Renderer* renderer, TextureManager* textureManager);
	void Shutdown();

	void GetOutputSize(int& width, int& height) const;
	void RenderTexture(SDL_Texture* texture, const SDL_Rect* dstRect) const;
	void RenderTexture(SDL_Texture* texture, const SDL_Rect* srcRect, const SDL_Rect* dstRect) const;
	void RenderTexture(const std::string& textureName, const SDL_Rect* dstRect) const;
	void RenderTexture(const std::string& textureName, const SDL_Rect* srcRect, const SDL_Rect* dstRect) const;

private:
	SDL_Renderer* m_renderer{ nullptr };
	TextureManager* m_textureManager{ nullptr };
};