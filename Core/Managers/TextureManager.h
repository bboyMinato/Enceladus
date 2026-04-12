#pragma once
#include <string>
#include <unordered_map>
#include <SDL2/SDL.h>

class TextureManager
{
public:
	~TextureManager();

	bool Init(SDL_Renderer* renderer);

	bool LoadTexture(const std::string& textureName, const std::string& filePath);

	void UnloadTexture(const std::string& textureName);
	void UnloadAllTextures();
	void GetTextureSize(SDL_Texture* texture, int& width, int& height) const;
	void GetTextureSize(const std::string& textureName, int& width, int& height) const;

	SDL_Texture* GetTexture(const std::string& textureName) const;
	
private:
	SDL_Renderer* m_renderer{ nullptr };
	std::unordered_map<std::string, SDL_Texture*> m_textures;
};