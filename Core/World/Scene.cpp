#include "Scene.h"
#include <SDL2/SDL_log.h>
#include <algorithm>

Entity Scene::CreateEntity(std::string_view key)
{
	if (!key.empty() && m_entities.contains(std::string(key)))
	{
		SDL_Log("Scene already contains an entity with key '%s'.", std::string(key).c_str());
		return {};
	}

	Entity entity = m_registry.CreateEntity();

	if (!key.empty())
	{
		m_entities.emplace(std::string(key), entity);
	}

	return entity;
}

std::string_view Scene::GetEntityKey(Entity entity) const
{
	if (!m_registry.IsValid(entity))
	{
		return {};
	}

	const auto it = std::find_if(m_entities.begin(), m_entities.end(),
	[entity](const auto& pair)
	{
		return pair.second == entity;
	});

	return it != m_entities.end() ? std::string_view(it->first) : std::string_view{};
}

Entity* Scene::FindEntity(std::string_view key)
{
	const auto it = m_entities.find(std::string(key));

	return it != m_entities.end() ? &it->second : nullptr;
}

const Entity* Scene::FindEntity(std::string_view key) const
{
	const auto it = m_entities.find(std::string(key));

	return it != m_entities.end() ? &it->second : nullptr;
}

void Scene::AddTexture(std::string_view textureName)
{
	m_textures.emplace_back(textureName);
}

void Scene::AddTexture(std::string_view textureName, const std::filesystem::path& filePath)
{
	AddTexture(textureName);
	m_textureFilePaths[std::string(textureName)] = filePath;
}

void Scene::AddSound(std::string_view soundName)
{
	m_sounds.emplace_back(soundName);
}

void Scene::AddSound(std::string_view soundName, const std::filesystem::path& filePath)
{
	AddSound(soundName);
	m_soundFilePaths[std::string(soundName)] = filePath;
}

void Scene::AddMusic(std::string_view musicName)
{
	m_music.emplace_back(musicName);
}

void Scene::AddMusic(std::string_view musicName, const std::filesystem::path& filePath)
{
	AddMusic(musicName);
	m_musicFilePaths[std::string(musicName)] = filePath;
}

void Scene::DestroyEntity(Entity entity)
{
	if (!m_registry.IsValid(entity))
	{
		return;
	}

	std::erase_if(m_entities, [&entity](const auto& pair)
	{
		return pair.second == entity;
	});

	m_registry.DestroyEntity(entity);
}

void Scene::Clear()
{
	m_registry.Clear();
	m_tileMap = {};
	m_entities.clear();
	m_textures.clear();
	m_sounds.clear();
	m_music.clear();
	m_textureFilePaths.clear();
	m_soundFilePaths.clear();
	m_musicFilePaths.clear();
	m_tileMapFilePath.clear();
	m_animationSetFilePath.clear();
}

bool Scene::RenameEntity(Entity entity, std::string_view newKey)
{
	if (!m_registry.IsValid(entity) || newKey.empty())
	{
		return false;
	}

	const std::string key(newKey);
	const auto existingKey = m_entities.find(key);

	if (existingKey != m_entities.end() && existingKey->second != entity)
	{
		SDL_Log("Scene already contains an entity with key '%s'.", key.c_str());
		return false;
	}

	if (existingKey != m_entities.end())
	{
		return true;
	}

	std::erase_if(m_entities, [&entity](const auto& pair)
	{
		return pair.second == entity;
	});

	m_entities.emplace(key, entity);
	return true;
}

void Scene::SetTileMapFilePath(std::filesystem::path filePath)
{
	m_tileMapFilePath = std::move(filePath);
}