#pragma once
#include <string>
#include <unordered_map>
#include <SDL2/SDL.h>
#include <filesystem>

class TextureManager
{
public:
	~TextureManager();

	/// <summary>
	/// Initializes the texture manager with the given SDL renderer.
	/// </summary>
	/// <param name="renderer">The SDL renderer to use for creating textures.</param>
	/// <returns>True if initialization was successful, false otherwise.</returns>
	bool Init(SDL_Renderer* renderer);

	/// <summary>
	/// Loads a texture from the specified file path and associates it with the given texture name.
	/// </summary>
	/// <param name="textureName">The name to associate with the loaded texture.</param>
	/// <param name="filePath">The file path of the texture to load.</param>
	/// <returns>True if the texture was loaded successfully, false otherwise.</returns>
	bool LoadTexture(std::string_view textureName, std::filesystem::path filePath);

	/// <summary>
	/// Unloads the texture associated with the given texture name.
	/// </summary>
	/// <param name="textureName">The name of the texture to unload.</param>
	void UnloadTexture(std::string_view textureName);

	/// <summary>
	/// Unloads all loaded textures.
	/// </summary>
	void UnloadAllTextures();

	/// <summary>
	/// Retrieves the size of the specified texture.
	/// </summary>
	/// <param name="texture">The texture to query.</param>
	/// <param name="width">The variable to store the width of the texture.</param>
	/// <param name="height">The variable to store the height of the texture.</param>
	void GetTextureSize(SDL_Texture* texture, int& width, int& height) const;

	/// <summary>
	/// Retrieves the size of the texture associated with the given texture name.
	/// </summary>
	/// <param name="textureName">The name of the texture to query.</param>
	/// <param name="width">The variable to store the width of the texture.</param>
	/// <param name="height">The variable to store the height of the texture.</param>
	void GetTextureSize(std::string_view textureName, int& width, int& height) const;

	/// <summary>
	/// Retrieves the texture associated with the given texture name.
	/// </summary>
	/// <param name="textureName">The name of the texture to retrieve.</param>
	/// <returns>The SDL_Texture associated with the given texture name, or nullptr if not found.</returns>
	SDL_Texture* GetTexture(std::string_view textureName) const;
	
private:
	SDL_Renderer* m_renderer{ nullptr };
	std::unordered_map<std::string, SDL_Texture*> m_textures;
};