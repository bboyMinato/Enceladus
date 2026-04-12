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

bool TextureManager::LoadTexture(const std::string& textureName, const std::string& filePath)
{
	SDL_Texture* texture = IMG_LoadTexture(m_renderer, filePath.c_str());
	if (!texture)
	{
		SDL_Log("Failed to load texture '%s': %s", filePath.c_str(), IMG_GetError());
		return false;
	}

	auto existingTextureIt = m_textures.find(textureName);
	if (existingTextureIt != m_textures.end())
	{
		SDL_DestroyTexture(existingTextureIt->second);
	}

	m_textures[textureName] = texture;

	return true;
}

void TextureManager::UnloadTexture(const std::string& textureName)
{
	auto it = m_textures.find(textureName);
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

void TextureManager::GetTextureSize(const std::string& textureName, int& width, int& height) const
{
	GetTextureSize(GetTexture(textureName), width, height);
}

SDL_Texture* TextureManager::GetTexture(const std::string& textureName) const
{
	const auto it = m_textures.find(textureName);
	
	if (it != m_textures.end())
	{
		return it->second;
	}

	return nullptr;
}
