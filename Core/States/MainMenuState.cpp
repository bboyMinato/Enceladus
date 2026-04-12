#include "MainMenuState.h"
#include "../Engine.h"
#include "PlayState.h"

void MainMenuState::OnEnter(Engine& engine)
{
	engine.GetTextureManager().LoadTexture("mainMenuBackground", "Assets/backgrounds/Enceladus_background.png");
	
	engine.GetSoundManager().LoadSound("menuHover", "Assets/sounds/Menu_Sound_Hover.wav");
	engine.GetSoundManager().LoadSound("menuClick", "Assets/sounds/Menu_Sound_Confirm.wav");

	engine.GetTextManager().LoadFont("menuFont", "Assets/fonts/Uncial.ttf", 48);
	engine.GetTextManager().LoadText("playText", "menuFont", "Play", { 255, 255, 255, 255 });
	engine.GetTextManager().LoadText("quitText", "menuFont", "Quit", { 255, 255, 255, 255 });

	m_menu.Clear();

	m_menu.AddMenuItem({ 540, 300, 300, 80 }, "playText", [&engine]()
		{
			engine.ReplaceState<PlayState>();
		});

	m_menu.AddMenuItem({ 540, 400, 300, 80 }, "quitText", [&engine]()
		{
			engine.RequestShutdown();
		});

	m_menu.SetOnSelectionChanged([&engine]()
		{
			engine.GetSoundManager().PlaySound("menuHover");
		});

	m_menu.SetOnItemActivated([&engine]()
		{
			engine.GetSoundManager().PlaySound("menuClick");
		});

	engine.GetSoundManager().LoadMusic("mainMenuMusic", "Assets/sounds/mainMenuMusic.ogg");
	engine.GetSoundManager().PlayMusic("mainMenuMusic");
}

void MainMenuState::OnExit(Engine& engine)
{
	engine.GetTextureManager().UnloadTexture("mainMenuBackground");
	engine.GetSoundManager().StopMusic();
	engine.GetSoundManager().UnloadMusic("mainMenuMusic");
	engine.GetTextManager().UnloadFont("menuFont");

	m_menu.Clear();
}

void MainMenuState::HandleEvent(Engine& engine, const SDL_Event& event)
{	
	(void)engine;

	m_menu.HandleEvent(event);
}

void MainMenuState::Update(Engine& engine, float deltaTime)
{
	(void)engine;
	(void)deltaTime;
}

void MainMenuState::Render(Engine& engine, SDL_Renderer* renderer)
{
	(void)engine;

	if (renderer == nullptr)
	{
		return;
	}

	RenderSystem& renderSystem = engine.GetRenderSystem();

	int windowWidth = 0;
	int windowHeight = 0;

	renderSystem.GetOutputSize(windowWidth, windowHeight);

	SDL_Rect rect{ 0, 0, windowWidth, windowHeight };

	renderSystem.RenderTexture("mainMenuBackground", nullptr, &rect);
	
	m_menu.Render(renderer, engine.GetTextManager());
}