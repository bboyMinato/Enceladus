#include "EditorState.h"
#include "../Engine.h"
#include <SDL2/SDL_log.h>
#include <format>
#include <optional>

EditorState::EditorState(std::filesystem::path scenePath)
	: m_scenePath(std::move(scenePath))
{
}

void EditorState::OnEnter(Engine& engine)
{
	const auto result = m_sceneManager.LoadScene(m_scenePath, engine);

	if (!result)
	{
		SDL_Log("Failed to load editor scene '%s': %s", m_scenePath.string().c_str(), result.error().c_str());

		engine.PopState();
		return;
	}

	m_editor.Initialize(engine, m_scenePath);
}

void EditorState::OnExit(Engine& engine)
{
	m_sceneManager.Unload(engine);
}

void EditorState::HandleEvent(Engine& engine, const SDL_Event& event)
{
	if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
	{
		if (m_editor.RequestClose())
		{
			engine.PopState();
		}
	}
}

void EditorState::Update(Engine& engine, float deltaTime)
{
	(void)engine;
	(void)deltaTime;
}

void EditorState::Render(Engine& engine, SDL_Renderer* renderer)
{
	(void)engine;
	(void)renderer;
}

#ifdef _DEBUG
void EditorState::RenderImGui(Engine& engine)
{
	Scene* scene = m_sceneManager.GetScene();

	if (scene == nullptr)
	{
		return;
	}

	m_editor.Render(engine, *scene);

	// Save As may have changed the active source path.
	m_scenePath = m_editor.GetScenePath();

	const std::optional<std::filesystem::path> requestedScenePath = m_editor.ConsumeOpenSceneRequest();

	if (!requestedScenePath.has_value())
	{
		return;
	}

	const auto result = m_sceneManager.LoadScene(*requestedScenePath, engine);

	if (result)
	{
		m_scenePath = *requestedScenePath;
		m_editor.Initialize(engine, m_scenePath);
		return;
	}

	const std::string openError = std::format("Failed to open scene: {}", result.error());

	// SceneManager unloads before loading. Restore the previous scene
	// so a failed Open request does not leave the editor black.
	const auto restoreResult = m_sceneManager.LoadScene(m_scenePath, engine);

	if (!restoreResult)
	{
		SDL_Log("Failed to restore editor scene '%s': %s", m_scenePath.string().c_str(), restoreResult.error().c_str());

		engine.PopState();
		return;
	}

	m_editor.Initialize(engine, m_scenePath);
	m_editor.SetErrorStatus(openError);
}
#endif