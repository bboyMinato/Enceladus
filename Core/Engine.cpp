#include "Engine.h"
#include "States/MainMenuState.h"

#ifdef _DEBUG
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#endif

Engine::Engine() 
	: m_stateManager(*this)
{
}

Engine::~Engine()
{
	Shutdown();
}
void Engine::Init()
{
	Init(m_config);
}

void Engine::Init(const EngineConfig& config)
{
	m_settings = AppSettings::LoadFromFile("config.ini");

	m_config = config;
	
	ApplyWindowSettings();

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
		SDL_Quit();

		return;
	}	

	m_window = SDL_CreateWindow(
		m_config.windowTitle, 
		m_config.windowPosX, 
		m_config.windowPosY, 
		m_config.windowWidth, 
		m_config.windowHeight, 
		m_config.windowFlags);

	if (!m_window)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		Shutdown();

		return;
	}

	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
	if (!m_renderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		Shutdown();

		return;
	}

	if (!m_textureManager.Init(m_renderer))
	{
		SDL_Log("Failed to initialize TextureManager.");
		Shutdown();
	
		return;
	}

	if (!m_textManager.Init(m_renderer))
	{
		SDL_Log("Failed to initialize TextManager.");
		Shutdown();
	
		return;
	}

	if (!m_soundManager.Init())
	{
		SDL_Log("Failed to initialize SoundManager.");
		Shutdown();

		return;
	}

	ApplyAudioSettings();

	if (!m_renderSystem.Init(m_renderer, &m_textureManager))
	{
		SDL_Log("Failed to initialize RenderSystem.");
		Shutdown();

		return;
	}

#ifdef _DEBUG
	if (!InitImGui())
	{
		SDL_Log("Failed to initialize ImGui.");
		Shutdown();
	
		return;
	}
#endif

	m_stateManager.PushState<MainMenuState>();

	m_lastFrameTicks = SDL_GetTicks64();
	m_isRunning = true;
}

void Engine::Run()
{
	while (m_isRunning)
	{
		m_stateManager.ApplyPendingStateChanges(*this);

		if (!m_stateManager.HasState())
		{
			m_isRunning = false;
			return;
		}

		const Uint32 frameStart = SDL_GetTicks64();
				
		m_deltaTime = (frameStart - m_lastFrameTicks) / 1000.0f;
		m_lastFrameTicks = frameStart;

		m_fps = m_deltaTime > 0.0f ? 1.0f / m_deltaTime : static_cast<float>(m_targetFPS);

		HandleEvents();
		Update(m_deltaTime);
		Render();

		Uint32 frameTime = SDL_GetTicks() - frameStart;
		if (frameTime < m_frameDelay)
		{
			SDL_Delay(m_frameDelay - frameTime);
		}
	}
}

void Engine::HandleEvents()
{
	m_inputSystem.BeginFrame();

	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
#ifdef _DEBUG
		if (m_isImGuiInitialized)
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
		}
#endif

		if (event.type == SDL_QUIT)
		{
			m_isRunning = false;
		}
		else if (event.type == SDL_WINDOWEVENT)
		{
			if (event.window.event == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			{
				m_config.windowWidth = event.window.data1;
				m_config.windowHeight = event.window.data2;
			}
			else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
			{
				m_hasFocus = true;
			}
			else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
			{
				m_hasFocus = false;
			}
		}

		bool blockedByImGui = false;

#ifdef _DEBUG
		if (m_isImGuiInitialized)
		{
			const ImGuiIO& io = ImGui::GetIO();

			const bool isMouseEvent =
				event.type == SDL_MOUSEMOTION ||
				event.type == SDL_MOUSEBUTTONDOWN ||
				event.type == SDL_MOUSEBUTTONUP ||
				event.type == SDL_MOUSEWHEEL;

			const bool isKeyboardEvent =
				event.type == SDL_KEYDOWN ||
				event.type == SDL_KEYUP ||
				event.type == SDL_TEXTINPUT;

			blockedByImGui =
				(isMouseEvent && io.WantCaptureMouse) ||
				(isKeyboardEvent && io.WantCaptureKeyboard);
		}
#endif

		if (!blockedByImGui)
		{
			m_inputSystem.ProcessEvent(event);
			m_stateManager.HandleEvent(*this, event);
		}
	}
}

void Engine::Update(float deltaTime)
{
	if (deltaTime < 0.0f)
	{
		return;
	}

	if (!m_hasFocus)
	{
		return;
	}

	m_stateManager.Update(*this, deltaTime);
}

void Engine::Render()
{
	if (!m_renderer || !m_hasFocus)
	{
		return;
	}

	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
	SDL_RenderClear(m_renderer);
	
#ifdef _DEBUG
	BeginImGuiFrame();
#endif

	m_stateManager.Render(*this, m_renderer);

#ifdef _DEBUG
	m_stateManager.RenderImGui(*this);
	RenderImGui();
#endif

	SDL_RenderPresent(m_renderer);
}

void Engine::Shutdown()
{
#ifdef _DEBUG
	ShutdownImGui();
#endif

	m_stateManager.Clear();
	m_stateManager.ApplyPendingStateChanges(*this);

	if (m_renderer)
	{	
		SDL_DestroyRenderer(m_renderer);
		m_renderer = nullptr;
	}

	if (m_window)
	{
		SDL_DestroyWindow(m_window);
		m_window = nullptr;
	}
}

void Engine::PopState()
{
	m_stateManager.Pop();
}

void Engine::ApplyWindowSettings()
{
	m_config.windowWidth = m_settings.windowSettings.width;
	m_config.windowHeight = m_settings.windowSettings.height;
	m_config.windowPosX = m_settings.windowSettings.posX;
	m_config.windowPosY = m_settings.windowSettings.posY;
	m_config.windowFlags = m_settings.windowSettings.flags;
}

void Engine::ApplyAudioSettings()
{
	m_soundManager.SetMusicVolume(m_settings.audioSettings.musicVolume);
	m_soundManager.SetSoundVolume(m_settings.audioSettings.soundVolume);
}

#ifdef _DEBUG
bool Engine::InitImGui()
{
	if (m_isImGuiInitialized)
	{
		return true;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	if (!ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer))
	{
		ImGui::DestroyContext();
		return false;
	}

	if (!ImGui_ImplSDLRenderer2_Init(m_renderer))
	{
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
		return false;
	}

	m_isImGuiInitialized = true;

	return true;
}

void Engine::ShutdownImGui()
{
	if (!m_isImGuiInitialized)
	{
		return;
	}

	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	m_isImGuiInitialized = false;	
}

void Engine::BeginImGuiFrame() const
{
	if (!m_isImGuiInitialized)
	{
		return;
	}

	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}

void Engine::RenderImGui()
{
	if (!m_isImGuiInitialized)
	{
		return;
	}

	ImGui::Render();
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), m_renderer);
}
#endif