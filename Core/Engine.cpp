#include "Engine.h"
#include "States/MainMenuState.h"

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
		m_inputSystem.ProcessEvent(event);


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

		m_stateManager.HandleEvent(*this, event);
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
	
	m_stateManager.Render(*this, m_renderer);

	SDL_RenderPresent(m_renderer);
}

void Engine::Shutdown()
{
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