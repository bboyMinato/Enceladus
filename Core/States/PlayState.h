#pragma once
#include "IState.h"	
#include "../ECS/Registry.h"
#include "../Managers/SceneManager.h"
#include "../Events/EventBus.h"
#include "../Utility/DialogueRuntimeState.h"

#ifdef _DEBUG
#include "../Utility/DebugHelpers.h"
#endif

class PlayState final : public IGameState
{
public:
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
	SceneManager m_sceneManager;
	Entity* m_player;
	Entity* m_camera;
	EventBus m_eventBus;
	DialogueRuntimeState m_dialogueState;

#ifdef _DEBUG
	DebugHelper m_debugHelper;
#endif
};
