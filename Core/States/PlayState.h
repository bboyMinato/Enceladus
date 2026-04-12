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

private:
	Registry m_registry;
	Entity m_player;
	Entity m_camera;
	TileMap m_tileMap;
};