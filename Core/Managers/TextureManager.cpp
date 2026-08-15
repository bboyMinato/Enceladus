#include "TextureManager.h"
#include <SDL2/SDL_image.h>

TextureManager::~TextureManager()
{
	UnloadAllTextures();
	IMG_Quit();

	m_renderer = nullptr;
}

bool TextureManager::Init(SDL_Renderer* renderer)
{
	if (!renderer)
	{
		SDL_Log("TextureManager initialization failed: Renderer is null.");
		return false;
	}

	m_renderer = renderer;

	const int imageFlags = IMG_INIT_PNG | IMG_INIT_JPG;

	if ((IMG_Init(imageFlags) & imageFlags) != imageFlags)
	{
		SDL_Log("Failed to initialize SDL_image: %s", IMG_GetError());

		return false;
	}

	return true;
}

bool TextureManager::LoadTexture(std::string_view textureName, std::string_view filePath)
{
	SDL_Texture* texture = IMG_LoadTexture(m_renderer, filePath.data());
	if (!texture)
	{
		SDL_Log("Failed to load texture '%s': %s", filePath.data(), IMG_GetError());
		return false;
	}

	// TODO: This destroys existing texture if the same name is used. Consider whether this is the desired behavior.
	auto existingTextureIt = m_textures.find(std::string(textureName));
	if (existingTextureIt != m_textures.end())
	{
		SDL_DestroyTexture(existingTextureIt->second);
	}

	m_textures[std::string(textureName)] = texture;

	return true;
}

void TextureManager::UnloadTexture(std::string_view textureName)
{
	auto it = m_textures.find(std::string(textureName));
	if (it != m_textures.end())
	{
		SDL_DestroyTexture(it->second);
		m_textures.erase(it);
	}
}

void TextureManager::UnloadAllTextures()
{
	for (auto& [name, texture] : m_textures)
	{
		(void)name; // Suppress unused variable warning
		SDL_DestroyTexture(texture);
	}

	m_textures.clear();
}

void TextureManager::GetTextureSize(SDL_Texture* texture, int& width, int& height) const
{
	if (!texture)
	{
		width = 0;
		height = 0;
		return;
	}

	SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
}

void TextureManager::GetTextureSize(std::string_view textureName, int& width, int& height) const
{
	GetTextureSize(GetTexture(textureName), width, height);
}

SDL_Texture* TextureManager::GetTexture(std::string_view textureName) const
{
	const auto it = m_textures.find(std::string(textureName));
	
	if (it != m_textures.end())
	{
		return it->second;
	}

	return nullptr;
}
