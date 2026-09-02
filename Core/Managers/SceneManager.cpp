#include "SceneManager.h"
#include "../Engine.h"

std::expected<bool, std::string> SceneManager::LoadScene(const std::filesystem::path& sceneFilePath, Engine& engine)
{
	Unload(engine);

	m_sceneLoadResult = SceneLoader::LoadScene(sceneFilePath, engine, m_registry, m_tileMap);

	if (!m_sceneLoadResult.loaded)
	{
		Unload(engine);

		return std::unexpected("Failed to load scene: " + sceneFilePath.string());
	}

	if (!m_sceneLoadResult.autoPlayedSound.empty())
	{
		engine.GetSoundManager().PlaySound(m_sceneLoadResult.autoPlayedSound);
	}

	if (!m_sceneLoadResult.autoPlayedMusic.empty())
	{
		engine.GetSoundManager().PlayMusic(m_sceneLoadResult.autoPlayedMusic);
	}

	return std::expected<bool, std::string>();
}

void SceneManager::Unload(Engine& engine)
{
	DestroySceneEntities();

	engine.GetSoundManager().StopMusic();

	for (const std::string& musicName : m_sceneLoadResult.loadedMusic)
	{
		engine.GetSoundManager().UnloadMusic(musicName);
	}

	for (const std::string& soundName : m_sceneLoadResult.loadedSounds)
	{
		engine.GetSoundManager().UnloadSound(soundName);
	}

	// This also unloads textures created for the scene tile map.
	engine.GetMapManager().Clear();

	for (const std::string& textureName : m_sceneLoadResult.loadedTextures)
	{
		engine.GetTextureManager().UnloadTexture(textureName);
	}

	engine.GetAnimationManager().Clear();

	m_tileMap = {};
	m_sceneLoadResult = {};
}

Entity SceneManager::FindEntity(std::string_view key) const
{
	const auto it = m_sceneLoadResult.entities.find(std::string(key));

	if (it == m_sceneLoadResult.entities.end())
	{
		return {};
	}

	return it->second;
}

#pragma region Private functions

void SceneManager::DestroySceneEntities()
{
	for (Entity entity : m_registry.GetAllEntities())
	{
		entity.Destroy();
	}
}

#pragma endregion