#pragma once
#include <SDL2/SDL.h>

class Engine;

class IGameState
{
public:
	virtual ~IGameState() = default;

	virtual void OnEnter(Engine& engine) {}
	virtual void OnExit(Engine& engine) {}
	virtual void OnPause(Engine& engine) {}
	virtual void OnResume(Engine& engine) {}

	virtual void HandleEvent(Engine& engine, const SDL_Event& event) = 0;
	virtual void Update(Engine& engine, float deltaTime) = 0;
	virtual void Render(Engine& engine, SDL_Renderer* renderer) = 0;
};