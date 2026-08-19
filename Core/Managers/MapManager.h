#pragma once
#include <string>
#include <optional>
#include <vector>
#include <filesystem>
#include "TextureManager.h"
#include "../World/TileMap.h"

class MapManager
{
public:
	MapManager(TextureManager& textureManager) : m_textureManager(textureManager) {}
	~MapManager();

	std::optional<TileMap> ParseFromTMJ(const std::filesystem::path& filePath);

	void Clear();

private:
	std::string SanitizeName(const std::string& value);
	std::string BuildTextureName(const std::filesystem::path& filePath, const TileSet& tileSet);
	std::string BuildImagePath(const std::filesystem::path& mapPath, const std::filesystem::path& imagePath);

	void UnloadTileMapTextures(const TileMap& tileMap);
private:
	std::vector<TileMap> m_tileMaps;

	TextureManager& m_textureManager;
};