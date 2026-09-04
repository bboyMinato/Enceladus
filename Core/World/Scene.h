#pragma once
#include "../ECS/Registry.h"
#include "TileMap.h"
#include <filesystem>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class Scene
{
public:
	[[nodiscard]]
	Registry& GetRegistry() { return m_registry; }

	[[nodiscard]]
	const Registry& GetRegistry() const { return m_registry; }
	
	[[nodiscard]]
	std::unordered_map<std::string, Entity>& GetEntities() { return m_entities; }

	[[nodiscard]]
	const std::unordered_map<std::string, Entity>& GetEntities() const { return m_entities; }

	[[nodiscard]]
	TileMap& GetTileMap() { return m_tileMap; }

	[[nodiscard]]
	const TileMap& GetTileMap() const { return m_tileMap; }

	Entity CreateEntity(std::string_view key = {});

	[[nodiscard]]
	std::string_view GetEntityKey(Entity entity) const;

	[[nodiscard]]
	Entity* FindEntity(std::string_view key);

	[[nodiscard]]
	const Entity* FindEntity(std::string_view key) const;

	void AddTexture(std::string_view textureName);

	void AddTexture(std::string_view textureName, const std::filesystem::path& filePath);

	[[nodiscard]] 
	const std::vector<std::string>& GetTextures() const { return m_textures; }

	void AddSound(std::string_view soundName);

	void AddSound(std::string_view soundName, const std::filesystem::path& filePath);

	[[nodiscard]]
	const std::vector<std::string>& GetSounds() const { return m_sounds; }

	[[nodiscard]]
	const std::unordered_map<std::string, std::filesystem::path>& GetSoundFilePaths() const { return m_soundFilePaths; }

	[[nodiscard]]
	const std::unordered_map<std::string, std::filesystem::path>& GetMusicFilePaths() const { return m_musicFilePaths; }

	void AddMusic(std::string_view musicName);
	
	void AddMusic(std::string_view musicName, const std::filesystem::path& filePath);

	[[nodiscard]]
	const std::vector<std::string>& GetMusic() const { return m_music; }
	
	void DestroyEntity(Entity entity);
	
	void Clear();

	bool RenameEntity(Entity entity, std::string_view newKey);

	[[nodiscard]]
	const std::unordered_map<std::string, std::filesystem::path>& GetTextureFilePaths() const { return m_textureFilePaths; }

	void SetTileMapFilePath(std::filesystem::path filePath);

	[[nodiscard]]
	const std::filesystem::path& GetTileMapFilePath() const { return m_tileMapFilePath; }

	void SetAnimationSetFilePath(std::filesystem::path filePath) { m_animationSetFilePath = std::move(filePath); }

	[[nodiscard]]
	const std::filesystem::path& GetAnimationSetFilePath() const { return m_animationSetFilePath; }
private:
	Registry m_registry{};
	TileMap m_tileMap{};

	std::unordered_map<std::string, Entity> m_entities;

	std::vector<std::string> m_textures;
	std::vector<std::string> m_sounds;
	std::vector<std::string> m_music;

	std::unordered_map<std::string, std::filesystem::path> m_textureFilePaths;
	std::unordered_map<std::string, std::filesystem::path> m_soundFilePaths;
	std::unordered_map<std::string, std::filesystem::path> m_musicFilePaths;

	std::filesystem::path m_tileMapFilePath;
	std::filesystem::path m_animationSetFilePath;
};