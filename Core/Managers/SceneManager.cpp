#include "SceneManager.h"
#include "../World/SceneLoader.h"
#include "../Engine.h"

std::expected<void, std::string> SceneManager::LoadScene(const std::filesystem::path& sceneFilePath, Engine& engine)
{
	Unload(engine);

	m_scene = std::make_unique<Scene>();

	auto result = SceneLoader::LoadScene(sceneFilePath, engine, *m_scene);

	if (!result)
	{
		Unload(engine);

		return std::unexpected(result.error());
	}

	return std::expected<void, std::string>();
}

void SceneManager::Unload(Engine& engine)
{
	if (!m_scene)
	{
		return;
	}

	DestroySceneEntities();

	engine.GetSoundManager().StopMusic();

	for (const std::string& musicName : m_scene->GetMusic())
	{
		engine.GetSoundManager().UnloadMusic(musicName);
	}

	for (const std::string& soundName : m_scene->GetSounds())
	{
		engine.GetSoundManager().UnloadSound(soundName);
	}

	engine.GetMapManager().Clear();

	for (const std::string& textureName : m_scene->GetTextures())
	{
		engine.GetTextureManager().UnloadTexture(textureName);
	}

	engine.GetAnimationManager().Clear();

	m_scene.reset();
}

#pragma region Private functions

void SceneManager::DestroySceneEntities()
{
	for (Entity entity : m_scene->GetRegistry().GetAllEntities())
	{
		entity.Destroy();
	}
}

#pragma endregion