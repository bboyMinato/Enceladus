#pragma once
#include "../ECS/Registry.h"
#include "TileMap.h"
#include <string>
#include <unordered_map>
#include <vector>

class Scene
{
public:
	[[nodiscard]]
	Registry& GetRegistry() { return m_registry; }

	[[nodiscard]]
	const Registry& GetRegistry() const { return m_registry; }
	
	[[nodiscard]]
	TileMap& GetTileMap() { return m_tileMap; }

	[[nodiscard]]
	const TileMap& GetTileMap() const { return m_tileMap; }

	Entity CreateEntity(std::string_view key = {});

	[[nodiscard]]
	Entity* FindEntity(std::string_view key);

	[[nodiscard]]
	const Entity* FindEntity(std::string_view key) const;

	void AddTexture(std::string_view textureName);

	[[nodiscard]] 
	const std::vector<std::string>& GetTextures() const { return m_textures; }

	void AddSound(std::string_view soundName);

	[[nodiscard]]
	const std::vector<std::string>& GetSounds() const { return m_sounds; }

	void AddMusic(std::string_view musicName);
	
	[[nodiscard]]
	const std::vector<std::string>& GetMusic() const { return m_music; }

private:
	Registry m_registry{};
	TileMap m_tileMap{};

	std::unordered_map<std::string, Entity> m_entities;

	std::vector<std::string> m_textures;
	std::vector<std::string> m_sounds;
	std::vector<std::string> m_music;
};