#pragma once
#include <SDL2/SDL.h>
#include "Managers/TextureManager.h"
#include "Managers/StateManager.h"
#include "Managers/SoundManager.h"
#include "Managers/TextManager.h"
#include "Systems/RenderSystem.h"
#include "Systems/InputSystem.h"
#include "Config/AppSetting.h"

struct EngineConfig
{
	int windowWidth{ 1480 };
	int windowHeight{ 900 };
	int windowPosX{ SDL_WINDOWPOS_CENTERED };
	int windowPosY{ SDL_WINDOWPOS_CENTERED };
	int windowFlags{ SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE };
	const char* windowTitle{ "Enceladus: The Journey and Beyond" };
};

class Engine
{
public:
	Engine();
	~Engine();

	void Init();
	void Init(const EngineConfig& config);
	void Run();
	void HandleEvents();
	void Update(float deltaTime);
	void Render();
	void Shutdown();
	void PopState();

	inline bool IsRunning() const { return m_isRunning; }
	inline EngineConfig GetConfig() const { return m_config; }
	inline TextureManager& GetTextureManager() { return m_textureManager; }
	inline RenderSystem& GetRenderSystem() { return m_renderSystem; }
	inline InputSystem& GetInputSystem() { return m_inputSystem; }
	inline SoundManager& GetSoundManager() { return m_soundManager; }
	inline TextManager& GetTextManager() { return m_textManager; }
	inline const AppSettings& GetAppSettings() const { return m_settings; }

	void RequestShutdown() { m_isRunning = false; }

	template<typename TState, typename... TArgs>
	void PushState(TArgs&&... args)
	{
		m_stateManager.PushState<TState>(std::forward<TArgs>(args)...);
	}

	template<typename TState, typename... TArgs>
	void ReplaceState(TArgs&&... args)
	{
		m_stateManager.ReplaceState<TState>(std::forward<TArgs>(args)...);
	}
	
private:
	void ApplyWindowSettings();
	void ApplyAudioSettings();

private:
	SDL_Window* m_window{ nullptr };
	SDL_Renderer* m_renderer{ nullptr };

	EngineConfig m_config;
	TextureManager m_textureManager;
	StateManager m_stateManager;
	RenderSystem m_renderSystem;
	InputSystem m_inputSystem;
	SoundManager m_soundManager;
	TextManager m_textManager;
	AppSettings m_settings;

	bool m_isRunning{ false };
	bool m_hasFocus{ true };

	Uint32 m_targetFPS{ 60 };
	Uint32 m_frameDelay{ 1000 / 60 };
	Uint32 m_lastFrameTicks{ 0 };

	float m_deltaTime{ 0.0f };
	float m_fps{ 0.0f };
};