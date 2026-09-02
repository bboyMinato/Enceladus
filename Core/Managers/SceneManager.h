#pragma once
#include <expected>
#include <string>
#include <filesystem>
#include <memory>
#include "../World/Scene.h"

class Engine;

class SceneManager final
{
public:
	std::expected<void, std::string> LoadScene(const std::filesystem::path& sceneFilePath, Engine& engine);
	void Unload(Engine& engine);

	[[nodiscard]]
	Scene* GetScene() { return m_scene.get(); }

	[[nodiscard]]
	const Scene* GetScene() const { return m_scene.get(); }

private:
	void DestroySceneEntities();

private:
	std::unique_ptr<Scene> m_scene;
};