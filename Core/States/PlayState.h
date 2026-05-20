#pragma once
#include "IState.h"	
#include "../ECS/Registry.h"
#include "../ECS/CameraComponent.h"
#include "../World/TileMap.h"

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
	Registry m_registry;
	Entity m_player;
	Entity m_camera;
	Entity m_testEntity;
	TileMap m_tileMap;

#ifdef _DEBUG
	bool m_showDebugWindow{ true };
	bool m_showColliderDebug{ true };
	int m_selectedDebugEntityIndex{ 0 };	
#endif
};