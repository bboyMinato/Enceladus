#pragma once
#include "../Managers/TextureManager.h"
#include "../Systems/RenderSystem.h"
#include <SDL2/SDL.h>
#include <cstdint>
#include <string>
#include <vector>

struct TileSet
{
	std::string name;
	std::string imagePath;
	std::string textureName;

	int firstGid{ 0 };
	int columns{ 0 };
	int tileWidth{ 0 };
	int tileHeight{ 0 };
	int tileCount{ 0 };
};

struct TileLayer
{
	std::string name; 

	int width{ 0 };
	int height{ 0 };

	bool isVisible{ true };

	std::vector<std::uint32_t> tiles;
};

class TileMap final
{
public:
	bool LoadFromTmj(const std::string& filePath, TextureManager& textureManager);
	void Clear(TextureManager* textureManager = nullptr);

	void Render(RenderSystem& renderSystem, const SDL_Rect& camera) const;

	bool IsLoaded() const;

	int GetMapWidth() const { return m_mapWidth; }
	int GetMapHeight() const { return m_mapHeight; }
	int GetTileWidth() const { return m_tileWidth; }
	int GetTileHeight() const { return m_tileHeight; }

	int GetPixelWidth() const { return m_mapWidth * m_tileWidth; }
	int GetPixelHeight() const { return m_mapHeight * m_tileHeight; }

private:
	const TileSet* FindTileSetForGid(std::uint32_t gid) const;
	void RenderLayer(RenderSystem& renderSystem, const TileLayer& layer, const SDL_Rect& camera) const;

private:
	std::string m_filePath;
	std::string m_orientation;

	int m_mapWidth{ 0 };
	int m_mapHeight{ 0 };
	int m_tileWidth{ 0 };
	int m_tileHeight{ 0 };

	std::vector<TileSet> m_tileSets;
	std::vector<TileLayer> m_tileLayers;
};