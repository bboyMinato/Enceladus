#pragma once
#include <expected>
#include <string>
#include <filesystem>
#include "../World/SceneLoader.h"
#include "../World/TileMap.h"
#include "../ECS/Registry.h"

class Engine;

class SceneManager final
{
public:
	std::expected<bool, std::string> LoadScene(const std::filesystem::path& sceneFilePath, Engine& engine);
	void Unload(Engine& engine);

	[[nodiscard]] 
	bool IsSceneLoaded() const { return m_sceneLoadResult.loaded; }

	[[nodiscard]]
	Registry& GetRegistry() { return m_registry; }

	[[nodiscard]]
	const Registry& GetRegistry() const { return m_registry; }

	[[nodiscard]]
	TileMap& GetTileMap() { return m_tileMap; }
	
	[[nodiscard]]
	const TileMap& GetTileMap() const { return m_tileMap; }

	[[nodiscard]]
	Entity FindEntity(std::string_view key) const;

private:
	void DestroySceneEntities();

private:
	Registry m_registry;
	TileMap m_tileMap;
	SceneLoadResult m_sceneLoadResult;
};