#pragma once
#include "IState.h"
#include "../UI/Menu.h"

class Engine;

class MainMenuState final : public IGameState
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
	Menu m_menu;
};