#pragma once
#include <SDL2/SDL.h>
#include "Managers/TextureManager.h"
#include "Managers/StateManager.h"
#include "Managers/SoundManager.h"
#include "Managers/TextManager.h"
#include "Systems/RenderSystem.h"
#include "Systems/InputSystem.h"
#include "Config/AppSetting.h"
#include "Managers/DialogManager.h"

struct EngineConfig
{
	int windowWidth{1480};
	int windowHeight{900};
	int windowPosX{SDL_WINDOWPOS_CENTERED};
	int windowPosY{SDL_WINDOWPOS_CENTERED};
	int windowFlags{SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE};
	const char* windowTitle{"Enceladus: The Journey and Beyond"};
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

	bool IsRunning() const { return m_isRunning; }
	EngineConfig GetConfig() const { return m_config; }
	TextureManager& GetTextureManager() { return m_textureManager; }
	RenderSystem& GetRenderSystem() { return m_renderSystem; }
	InputSystem& GetInputSystem() { return m_inputSystem; }
	SoundManager& GetSoundManager() { return m_soundManager; }
	TextManager& GetTextManager() { return m_textManager; }
	DialogManager& GetDialogManager() { return m_dialogManager; }
	const AppSettings& GetAppSettings() const { return m_settings; }

	void RequestShutdown() { m_isRunning = false; }

	template <typename TState, typename... TArgs>
	void PushState(TArgs&&... args)
	{
		m_stateManager.PushState<TState>(std::forward<TArgs>(args)...);
	}

	template <typename TState, typename... TArgs>
	void ReplaceState(TArgs&&... args)
	{
		m_stateManager.ReplaceState<TState>(std::forward<TArgs>(args)...);
	}

private:
	void ApplyWindowSettings();
	void ApplyAudioSettings();

#ifdef _DEBUG
	bool InitImGui();
	void ShutdownImGui();
	void BeginImGuiFrame() const;
	void RenderImGui();
#endif

	SDL_Window* m_window{nullptr};
	SDL_Renderer* m_renderer{nullptr};

	EngineConfig m_config;
	TextureManager m_textureManager;
	StateManager m_stateManager;
	RenderSystem m_renderSystem;
	InputSystem m_inputSystem;
	SoundManager m_soundManager;
	TextManager m_textManager;
	DialogManager m_dialogManager;
	AppSettings m_settings;

	bool m_isRunning{false};
	bool m_hasFocus{true};

	Uint32 m_targetFPS{60};
	Uint32 m_frameDelay{1000 / 60};
	Uint32 m_lastFrameTicks{0};

	float m_deltaTime{0.0f};
	float m_fps{0.0f};

#ifdef _DEBUG
	bool m_isImGuiInitialized{false};
	
#endif
};
