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

struct TileMap
{
	std::string m_orientation;

	int m_mapWidth{ 0 };
	int m_mapHeight{ 0 };
	int m_tileWidth{ 0 };
	int m_tileHeight{ 0 };

	std::vector<TileSet> tileSets;
	std::vector<TileLayer> tileLayers;

	[[nodiscard]]
	int GetPixelWidth() const { return m_mapWidth * m_tileWidth; }
	
	[[nodiscard]]
	int GetPixelHeight() const { return m_mapHeight * m_tileHeight; }
};