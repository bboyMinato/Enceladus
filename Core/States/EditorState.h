#pragma once
#include "../Editor/Editor.h"
#include "../Managers/SceneManager.h"
#include "IState.h"
#include <filesystem>

class Engine;

class EditorState final : public IGameState
{
public:
	explicit EditorState(std::filesystem::path scenePath = "scenes/play_scene.json");

	void OnEnter(Engine& engine) override;
	void OnExit(Engine& engine) override;
	void OnPause(Engine& engine) override {}
	void OnResume(Engine& engine) override {}

	void HandleEvent(Engine& engine, const SDL_Event& event) override;
	void Update(Engine& engine, float deltaTime) override;
	void Render(Engine& engine, SDL_Renderer* renderer) override;

#ifdef _DEBUG
	void RenderImGui(Engine& engine) override;
#endif

private:
	std::filesystem::path m_scenePath;
	SceneManager m_sceneManager;
	Editor m_editor;
};