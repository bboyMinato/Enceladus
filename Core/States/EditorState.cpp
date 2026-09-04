#include "EditorState.h"
#include "../Engine.h"
#include <SDL2/SDL_log.h>

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

	if (scene != nullptr)
	{
		m_editor.Render(engine, *scene);
	}
}
#endif