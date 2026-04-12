#pragma once
#include <SDL2/SDL.h>
#include <string>

struct SpriteComponent
{
	SpriteComponent() = default;
	SpriteComponent(const std::string& textureName, int width, int height)
		: m_textureName(textureName), m_width(width), m_height(height), m_hasSourceRect(false)
	{
	}

	SpriteComponent(const std::string& textureName, const SDL_Rect& sourceRect, int width, int height)
		: m_textureName(textureName), m_sourceRect(sourceRect), m_hasSourceRect(true), m_width(width), m_height(height)
	{
	}

	std::string m_textureName;
	SDL_Rect m_sourceRect{ 0, 0, 0, 0 };

	int m_width{ 0 };
	int m_height{ 0 };

	bool m_isVisible{ true };
	bool m_hasSourceRect{ false };
	SDL_RendererFlip m_flip{ SDL_FLIP_NONE };
};