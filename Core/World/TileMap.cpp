#include "TileMap.h"
#include <nlohmann/json.hpp>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>

using Json = nlohmann::json;

namespace
{
	constexpr std::uint32_t HorizontalFlipFlag = 0x80000000u;
	constexpr std::uint32_t VerticalFlipFlag = 0x40000000u;
	constexpr std::uint32_t DiagonalFlipFlag = 0x20000000u;

	std::uint32_t StripTileFlags(std::uint32_t gid)
	{
		return gid & ~(HorizontalFlipFlag | VerticalFlipFlag | DiagonalFlipFlag);
	}

	std::string SanitizeName(const std::string& value)
	{
		std::string result;
		result.reserve(value.size());

		for (const char character : value)
		{
			if (std::isalnum(static_cast<unsigned char>(character)) != 0)
			{
				result.push_back(character);
			}
			else
			{
				result.push_back('_');
			}
		}

		return result;
	}

	std::string BuildTextureName(const std::string& mapFilePath, const TileSet& tileSet)
	{
		const std::filesystem::path mapPath(mapFilePath);

		return "tilemap_" +
			SanitizeName(mapPath.stem().string()) +
			"_" +
			std::to_string(tileSet.firstGid) +
			"_" +
			SanitizeName(tileSet.name);
	}

	std::string BuildImagePath(const std::string& mapFilePath, const std::string& imagePath)
	{
		const std::filesystem::path image(imagePath);

		if (image.is_absolute())
		{
			return image.lexically_normal().string();
		}

		const std::filesystem::path mapDirectory = std::filesystem::path(mapFilePath).parent_path();
		return (mapDirectory / image).lexically_normal().string();
	}
}

bool TileMap::LoadFromTmj(const std::string& filePath, TextureManager& textureManager)
{
	Clear(&textureManager);

	std::ifstream file(filePath);
	if (!file.is_open())
	{
		SDL_Log("Failed to open tile map file: %s", filePath.c_str());

		return false;
	}

	Json document;

	try
	{
		file >> document;
	}
	catch (const Json::parse_error& e)
	{
		SDL_Log("Failed to parse tile map JSON: %s", e.what());

		return false;
	}

	m_orientation = document.value("orientation", "");
	m_mapWidth = document.value("width", 0);
	m_mapHeight = document.value("height", 0);
	m_tileWidth = document.value("tilewidth", 0);
	m_tileHeight = document.value("tileheight", 0);

	if (m_orientation != "orthogonal")
	{
		SDL_Log("Only orthogonal tile maps are supported: %s", filePath.c_str());
		Clear();

		return false;
	}

	if (m_mapWidth <= 0 || m_mapHeight <= 0 || m_tileWidth <= 0 || m_tileHeight <= 0)
	{
		SDL_Log("Invalid tile map dimensions in file: %s", filePath.c_str());
		Clear();

		return false;
	}

	if (!document.contains("tilesets") || !document["tilesets"].is_array())
	{
		SDL_Log("Tile map file does not contain a valid tilesets array: %s", filePath.c_str());
		Clear();

		return false;
	}

	for (const Json& tileSetJson : document["tilesets"])
	{
		TileSet tileSet;
		tileSet.name = tileSetJson.value("name", "");
		tileSet.imagePath = tileSetJson.value("image", "");
		tileSet.firstGid = tileSetJson.value("firstgid", 0);
		tileSet.tileWidth = tileSetJson.value("tilewidth", 0);
		tileSet.tileHeight = tileSetJson.value("tileheight", 0);
		tileSet.tileCount = tileSetJson.value("tilecount", 0);
		tileSet.columns = tileSetJson.value("columns", 0);

		if (tileSet.name.empty() ||
			tileSet.imagePath.empty() ||
			tileSet.firstGid <= 0 ||
			tileSet.tileWidth <= 0 ||
			tileSet.tileHeight <= 0 ||
			tileSet.tileCount <= 0 ||
			tileSet.columns <= 0)
		{
			SDL_Log("Invalid tileset definition in file: %s", filePath.c_str());
			Clear(&textureManager);

			return false;
		}

		tileSet.imagePath = BuildImagePath(filePath, tileSet.imagePath);
		tileSet.textureName = BuildTextureName(filePath, tileSet);

		if (!textureManager.LoadTexture(tileSet.textureName, tileSet.imagePath))
		{
			SDL_Log("Failed to load tileset texture: %s", tileSet.imagePath.c_str());
			Clear(&textureManager);

			return false;
		}

		m_tileSets.push_back(tileSet);
	}

	std::sort(m_tileSets.begin(), m_tileSets.end(),
		[](const TileSet& a, const TileSet& b)
		{
			return a.firstGid < b.firstGid;
		});

	if (!document.contains("layers") || !document["layers"].is_array())
	{
		SDL_Log("Tile map file does not contain valid layers array: %s", filePath.c_str());
		Clear(&textureManager);

		return false;
	}

	for (const Json& layerJson : document["layers"])
	{
		if (layerJson.value("type", "") != "tilelayer")
		{
			continue;
		}

		TileLayer layer;
		layer.name = layerJson.value("name", "");
		layer.width = layerJson.value("width", m_mapWidth);
		layer.height = layerJson.value("height", m_mapHeight);
		layer.isVisible = layerJson.value("visible", true);

		if (!layerJson.contains("data") || !layerJson["data"].is_array())
		{
			SDL_Log("TileMap load failed: layer '%s' is missing tile data.", layer.name.c_str());
			Clear(&textureManager);

			return false;
		}

		try
		{
			layer.tiles = layerJson["data"].get<std::vector<std::uint32_t>>();
		}
		catch (const Json::type_error&)
		{
			SDL_Log("TileMap load failed: layer '%s' contains invalid tile data.", layer.name.c_str());
			Clear(&textureManager);

			return false;
		}

		const size_t expectedTileCount = static_cast<size_t>(layer.width) * static_cast<size_t>(layer.height);
		if (layer.tiles.size() != expectedTileCount)
		{
			SDL_Log(
				"TileMap load failed: layer '%s' tile data size does not match expected count (%zu).",
				layer.name.c_str(),
				expectedTileCount);

			Clear(&textureManager);

			return false;
		}

		m_tileLayers.push_back(TileLayer
		{
			layer.name,
			layer.width,
			layer.height,
			layer.isVisible,
			std::move(layer.tiles)
		});
	}

	if (m_tileLayers.empty())
	{
		SDL_Log("Tile map file does not contain any valid tile layers: %s", filePath.c_str());
		Clear(&textureManager);

		return false;
	}

	m_filePath = filePath;

	return true;
}

void TileMap::Clear(TextureManager* textureManager)
{
	if (textureManager != nullptr)
	{
		for (const TileSet& tileSet : m_tileSets)
		{
			if (!tileSet.textureName.empty())
			{
				textureManager->UnloadTexture(tileSet.textureName);
			}
		}
	}

	m_filePath.clear();
	m_orientation.clear();
	m_mapWidth = 0;
	m_mapHeight = 0;
	m_tileWidth = 0;
	m_tileHeight = 0;
	m_tileSets.clear();
	m_tileLayers.clear();
}

void TileMap::Render(RenderSystem& renderSystem, const SDL_Rect& camera) const
{
	if (!IsLoaded())
	{
		return;
	}

	for (const TileLayer& layer : m_tileLayers)
	{
		if (!layer.isVisible)
		{
			continue;
		}

		RenderLayer(renderSystem, layer, camera);
	}
}

bool TileMap::IsLoaded() const
{
	return m_mapWidth > 0 &&
		m_mapHeight > 0 &&
		m_tileWidth > 0 &&
		m_tileHeight > 0 &&
		!m_tileSets.empty() &&
		!m_tileLayers.empty();
}

const TileSet* TileMap::FindTileSetForGid(std::uint32_t gid) const
{
	gid = StripTileFlags(gid);

	if (gid == 0)
	{
		return nullptr;
	}

	for (auto it = m_tileSets.rbegin(); it != m_tileSets.rend(); ++it)
	{
		if (gid >= static_cast<std::uint32_t>(it->firstGid))
		{
			const std::uint32_t localTileId = gid - static_cast<std::uint32_t>(it->firstGid);

			if (localTileId < static_cast<std::uint32_t>(it->tileCount))
			{
				return &(*it);
			}
		}
	}

	return nullptr;
}

void TileMap::RenderLayer(RenderSystem& renderSystem, const TileLayer& layer, const SDL_Rect& camera) const
{
	const int startColumn = std::max(0, camera.x / m_tileWidth);
	const int endColumn = std::min(layer.width, ((camera.x + camera.w) / m_tileWidth) + 1);

	const int startRow = std::max(0, camera.y / m_tileHeight);
	const int endRow = std::min(layer.height, ((camera.y + camera.h) / m_tileHeight) + 1);

	for (int row = startRow; row < endRow; ++row)
	{
		for (int column = startColumn; column < endColumn; ++column)
		{
			const size_t tileIndex = static_cast<size_t>(row) * static_cast<size_t>(layer.width) + static_cast<size_t>(column);
			const std::uint32_t gid = StripTileFlags(layer.tiles[tileIndex]);

			if (gid == 0)
			{
				continue;
			}

			const TileSet* tileSet = FindTileSetForGid(gid);
			if (tileSet == nullptr)
			{
				continue;
			}

			const int localTileId = static_cast<int>(gid) - tileSet->firstGid;

			SDL_Rect srcRect
			{
				(localTileId % tileSet->columns) * tileSet->tileWidth,
				(localTileId / tileSet->columns) * tileSet->tileHeight,
				tileSet->tileWidth,
				tileSet->tileHeight
			};

			SDL_Rect dstRect
			{
				(column * m_tileWidth) - camera.x,
				(row * m_tileHeight) - camera.y,
				m_tileWidth,
				m_tileHeight
			};

			renderSystem.RenderTexture(tileSet->textureName, &srcRect, &dstRect);
		}
	}
}
