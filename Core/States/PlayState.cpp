#include "PlayState.h"
#include "../ECS/ColliderComponent.h"
#include "../ECS/ControllerComponent.h"
#include "../ECS/InteractableComponent.h"
#include "../ECS/MovementComponent.h"
#include "../ECS/SpriteComponent.h"
#include "../ECS/TagComponent.h"
#include "../ECS/TransformComponent.h"
#include "../Engine.h"
#include "../Events/Handlers.h"
#include "../Systems/AnimationStateSystem.h"
#include "../Systems/CameraSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/ControllerSystem.h"
#include "../Systems/DialogueSystem.h"
#include "../Systems/MapConstraintSystem.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/SpriteAnimationSystem.h"
#include "../Utility/DebugHelpers.h"

#ifdef _DEBUG
#include <imgui.h>
#include <vector>
#endif

void PlayState::OnEnter(Engine& engine)
{
	auto result = m_sceneManager.LoadScene("scenes/play_scene.json", engine);

	if (!result)
	{
		SDL_Log("Failed to load scene: %s", result.error().c_str());
		return;
	}

	Scene* scene = m_sceneManager.GetScene();

	if (scene == nullptr)
	{
		return;
	}

	engine.GetSoundManager().PlayMusic("background_forest_music", -1);

	m_player = scene->FindEntity("player");
	m_camera = scene->FindEntity("player_camera");

	if (!m_player || !m_camera)
	{
		SDL_Log("Scene loaded but player or camera entity is missing.");

		m_sceneManager.Unload(engine);
		return;
	}

	CameraComponent* camera = m_camera->Get<CameraComponent>();
	
	if (camera == nullptr)
	{
		SDL_Log("Scene camera entity is missing CameraComponent.");
		m_sceneManager.Unload(engine);
		m_player = {};
		m_camera = {};
		return;
	}
	
	camera->m_target = *m_player;
	camera->m_viewport.w = engine.GetConfig().windowWidth;
	camera->m_viewport.h = engine.GetConfig().windowHeight;

    engine.GetTextManager().LoadFont("menuFont", "fonts/Uncial.ttf", 48);
    engine.GetTextManager().LoadFont("dialogueFont", "fonts/dialogueFont.ttf", 42);

	if (!engine.GetDialogueManager().LoadDialogue("dialogues/test_rework.json"))
	{
		SDL_Log("Failed to load dialogue: test_rework.json");
	}

	SetupInteractionHandlers(m_eventBus,  engine.GetDialogueManager(), m_dialogueState);
}

void PlayState::OnExit(Engine& engine)
{
	m_player = {};
	m_camera = {};

	m_sceneManager.Unload(engine);

	engine.GetTextManager().UnloadFont("menuFont");
	engine.GetTextManager().UnloadFont("dialogueFont");
}

void PlayState::HandleEvent(Engine& engine, const SDL_Event& event)
{
	(void)engine;
	(void)event;
}

void PlayState::Update(Engine& engine, float deltaTime)
{
	Scene* scene = m_sceneManager.GetScene();

	if (scene == nullptr || !m_player || !m_camera)
	{
		engine.PopState();
		return;
	}

	const InputSystem& input = engine.GetInputSystem();

	if (m_dialogueState.m_isActive)
	{
		DialogueSystem::Update(m_dialogueState, input);
		AnimationSystem::Update(scene->GetRegistry(), engine.GetAnimationManager(), deltaTime);

		return;
	}

	if (ControllerSystem::PopState(scene->GetRegistry(), input))
	{
		engine.PopState();
		return;
	}

	ControllerSystem::Update(scene->GetRegistry(), input, m_eventBus);

	MovementSystem::Update(scene->GetRegistry(), deltaTime);
	CollisionSystem::Update(scene->GetRegistry());

	TransformComponent* transform = m_player->Get<TransformComponent>();
	const SpriteComponent* sprite = m_player->Get<SpriteComponent>();

	MapConstraintSystem::ClampToTileMap(*transform, *sprite, scene->GetTileMap());
	AnimationStateSystem::UpdateAnimationStates(scene->GetRegistry(), engine.GetAnimationManager());
	AnimationSystem::Update(scene->GetRegistry(), engine.GetAnimationManager(), deltaTime);
	CameraSystem::Update(scene->GetRegistry(), *m_camera, engine.GetRenderSystem(), scene->GetTileMap());
}

void PlayState::Render(Engine &engine, SDL_Renderer *renderer)
{
	Scene* scene = m_sceneManager.GetScene();

	if (scene == nullptr || !m_player || !m_camera)
	{
		return;
	}

    if (renderer == nullptr)
    {
        return;
    }

    const CameraComponent* camera = m_camera->Get<CameraComponent>();
    if (camera == nullptr)
    {
        return;
    }

    auto& renderSystem = engine.GetRenderSystem();

	auto& mapSystem = engine.GetMapSystem();
	mapSystem.Render(scene->GetTileMap(), renderSystem, *camera, scene->GetRegistry());

	int windowWidth = 0;
	int windowHeight = 0;
	renderSystem.GetOutputSize(windowWidth, windowHeight);

	renderSystem.RenderDialogue(m_dialogueState, windowWidth, windowHeight, engine.GetTextManager());
}

#ifdef _DEBUG
void PlayState::RenderImGui(Engine& engine)
{
	Scene* scene = m_sceneManager.GetScene();

	if (scene == nullptr || !m_player || !m_camera)
	{
		return;
	}

	m_debugHelper.RenderImGui(engine, scene->GetRegistry());
	m_debugHelper.RenderDebugCollider(engine, scene->GetRegistry(), *m_camera);
}

#endif