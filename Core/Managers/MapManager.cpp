#include "MapManager.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include "../Utility/AssetFilePaths.h"

using Json = nlohmann::json;

MapManager::~MapManager()
{
	Clear();
}

std::optional<TileMap> MapManager::ParseFromTMJ(const std::filesystem::path& filePath)
{
	const std::filesystem::path resolvedPath = AssetPaths::ResolveAsset(filePath);

	if (resolvedPath.extension() != ".tmj")
	{
		SDL_Log("Invalid tile map file extension: %s", resolvedPath.string().c_str());

		return std::nullopt;
	}

	std::ifstream file(resolvedPath);
	if (!file.is_open())
	{
		SDL_Log("Failed to open tile map file: %s", resolvedPath.string().c_str());
		
		return std::nullopt;
	}

	Json document = Json::parse(file, nullptr, false);

	if (document.is_discarded())
	{
		SDL_Log("Failed to parse scene JSON file: %s", resolvedPath.string().c_str());

		return std::nullopt;
	}

	TileMap tileMap
	{
		.m_orientation = document.value("orientation", ""),
		.m_mapWidth = document.value("width", 0),
		.m_mapHeight = document.value("height", 0),
		.m_tileWidth = document.value("tilewidth", 0),
		.m_tileHeight = document.value("tileheight", 0)
	};

	const auto fail = [this, &tileMap]()
		{
			UnloadTileMapTextures(tileMap);

			return std::optional<TileMap>{};
		};

	if (tileMap.m_orientation != "orthogonal")
	{
		SDL_Log("Only orthogonal tile maps are supported: %s", resolvedPath.string().c_str());

		return fail();
	}

	if (tileMap.m_mapWidth <= 0 || tileMap.m_mapHeight <= 0 || tileMap.m_tileWidth <= 0 || tileMap.m_tileHeight <= 0)
	{
		SDL_Log("Invalid tile map dimensions in file: %s", resolvedPath.string().c_str());

		return fail();
	}

	if (!document.contains("tilesets") || !document["tilesets"].is_array())
	{
		SDL_Log("Tile map file does not contain a valid tilesets array: %s", resolvedPath.string().c_str());

		return fail();
	}

	for (const Json& tileSetDef : document["tilesets"])
	{
		TileSet tileSet
		{
			.name = tileSetDef.value("name", ""),
			.imagePath = tileSetDef.value("image", ""),
			.firstGid = tileSetDef.value("firstgid", 0),
			.columns = tileSetDef.value("columns", 0),
			.tileWidth = tileSetDef.value("tilewidth", 0),
			.tileHeight = tileSetDef.value("tileheight", 0),
			.tileCount = tileSetDef.value("tilecount", 0)
		};

		tileSet.imagePath = BuildImagePath(resolvedPath, tileSet.imagePath);
		tileSet.textureName = BuildTextureName(resolvedPath, tileSet);

		if (!m_textureManager.LoadTexture(tileSet.textureName, tileSet.imagePath))
		{
			SDL_Log("Failed to load texture: %s", tileSet.textureName.c_str());

			return fail();
		}

		tileMap.tileSets.push_back(std::move(tileSet));
	}

	std::sort(tileMap.tileSets.begin(), tileMap.tileSets.end(),
		[](const TileSet& a, const TileSet& b)
		{
			return a.firstGid < b.firstGid;
		});

	if (!document.contains("layers") || !document["layers"].is_array())
	{
		SDL_Log("Tile map file does not contain valid layers array: %s", filePath.string().c_str());

		return fail();
	}

	for (const Json& layerDef : document["layers"])
	{
		if (layerDef.value("type", "") != "tilelayer")
		{
			continue;
		}

		TileLayer layer
		{
			.name = layerDef.value("name", ""),
			.width = layerDef.value("width", tileMap.m_mapWidth),
			.height = layerDef.value("height", tileMap.m_mapHeight),
			.isVisible = layerDef.value("visible", true)
		};

		const Json& dataJson = layerDef["data"];

		if (!dataJson.is_array())
		{
			SDL_Log("Tile layer '%s' does not contain valid data array.", layer.name.c_str());

			return fail();
		}

		layer.tiles.reserve(dataJson.size());

		for (const auto& element : dataJson)
		{
			if (!element.is_number_unsigned())
			{
				SDL_Log("Tile layer '%s' contains non-unsigned integer tile ID.", layer.name.c_str());
			
				return fail();
			}

			layer.tiles.push_back(element.get<std::uint32_t>());
		}

		const size_t expectedTileCount = static_cast<size_t>(layer.width) * static_cast<size_t>(layer.height);

		if (layer.tiles.size() != expectedTileCount)
		{
			SDL_Log("Tile layer '%s' tile data size does not match expected count (%zu).", layer.name.c_str(), expectedTileCount);
			
			return fail();
		}

		tileMap.tileLayers.push_back(std::move(layer));
	}

	if (tileMap.tileLayers.empty())
	{
		SDL_Log("Tile map file does not contain any valid tile layers: %s", filePath.string().c_str());
		
		return fail();
	}

	m_tileMaps.push_back(tileMap);

	return tileMap;
}

void MapManager::Clear()
{
	for (const auto& tileMap : m_tileMaps)
	{
		UnloadTileMapTextures(tileMap);
	}

	m_tileMaps.clear();
}

#pragma region Private functions

std::string MapManager::SanitizeName(const std::string& value)
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

std::string MapManager::BuildTextureName(const std::filesystem::path& filePath, const TileSet& tileSet)
{
	std::string textureName = std::format("tilemap_{}_{}_{}", filePath.stem().string(), tileSet.firstGid, tileSet.name);

	return textureName;
}

std::string MapManager::BuildImagePath(const std::filesystem::path& mapPath, const std::filesystem::path& imagePath)
{
	if (imagePath.is_absolute())
	{
		return imagePath.lexically_normal().string();
	}

	return (mapPath.parent_path() / imagePath).lexically_normal().string();
}

void MapManager::UnloadTileMapTextures(const TileMap& tileMap)
{
	for (const TileSet& tileSet : tileMap.tileSets)
	{
		if (!tileSet.textureName.empty())
		{
			m_textureManager.UnloadTexture(tileSet.textureName);
		}
	}
}

#pragma endregion