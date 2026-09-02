#include "Scene.h"
#include <SDL2/SDL_log.h>

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


Entity Scene::FindEntity(std::string_view key) const
{
	const auto it = m_entities.find(std::string(key));

	return it != m_entities.end() ? it->second : Entity();
}

void Scene::AddTexture(std::string_view textureName)
{
	m_textures.emplace_back(textureName);
}

void Scene::AddSound(std::string_view soundName)
{
	m_sounds.emplace_back(soundName);
}

void Scene::AddMusic(std::string_view musicName)
{
	m_music.emplace_back(musicName);
}