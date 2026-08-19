#include "PlayState.h"
#include "../Engine.h"
#include "../ECS/SpriteComponent.h"
#include "../ECS/TransformComponent.h"
#include "../ECS/ControllerComponent.h"
#include "../ECS/MovementComponent.h"
#include "../ECS/ColliderComponent.h"
#include "../ECS/TagComponent.h"
#include "../ECS/InteractableComponent.h"
#include "../Systems/AnimationStateSystem.h"
#include "../Systems/ControllerSystem.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/MapConstraintSystem.h"
#include "../Systems/CameraSystem.h"
#include "../Systems/SpriteAnimationSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Utility/DebugHelpers.h"
#include "../World/SceneLoader.h"
#include "../Events/Handlers.h"
#include <algorithm>
#include <string>

#ifdef _DEBUG
#include <imgui.h>
#include <vector>
#endif

void PlayState::OnEnter(Engine& engine)
{
	SceneLoader loader;
	const SceneLoadResult result = loader.LoadScene("Assets/play_scene.json", engine, m_registry, m_tileMap);

	if (!result.loaded)
	{
		SDL_Log("Failed to load scene.");
		return;
	}

	if (const auto playerIt = result.entities.find("player"); playerIt != result.entities.end())
	{
		m_player = playerIt->second;
	}

	if (const auto cameraIt = result.entities.find("main_camera"); cameraIt != result.entities.end())
	{
		m_camera = cameraIt->second;
	}

	if (!m_player || !m_camera)
	{
		SDL_Log("Scene loaded but player or camera entity is missing.");
		return;
	}

	engine.GetSoundManager().PlayMusic("background_forest_music", true);
    engine.GetTextManager().LoadFont("menuFont", "Assets/fonts/Uncial.ttf", 48);

	SetupInteractionHandlers(m_eventBus, m_registry, engine.GetDialogManager());
}

void PlayState::OnExit(Engine& engine)
{
	if (m_player)
	{
		m_player.Destroy();
	}

	if (m_camera)
	{
		m_camera.Destroy();
		m_camera = {};
	}

	engine.GetDialogManager().ClearDialogue();

	engine.GetAnimationManager().Clear();

	engine.GetTextureManager().UnloadTexture("player_idle");
	engine.GetTextureManager().UnloadTexture("player_v2_idle");
	engine.GetTextureManager().UnloadTexture("player_walk");
	engine.GetTextureManager().UnloadTexture("player_v2_walk");
	engine.GetSoundManager().UnloadSound("background_forest_music");
	engine.GetMapManager().Clear();

	m_tileMap = {};
}

void PlayState::HandleEvent(Engine& engine, const SDL_Event& event)
{
	(void)engine;
	(void)event;
}

void PlayState::Update(Engine& engine, float deltaTime)
{
	const InputSystem& input = engine.GetInputSystem();
	DialogueManager& dialogManager = engine.GetDialogManager();

	if (ControllerSystem::PopState(m_registry, input))
	{
		engine.PopState();
		return;
	}

	dialogManager.Update(deltaTime);

	if (dialogManager.HasActiveDialogue())
	{
		if (const ControllerComponent* controller = m_player.Get<ControllerComponent>(); controller != nullptr)
		{
			if (input.WasKeyPressed(controller->interactPrimary) || input.WasKeyPressed(controller->interactSecondary))
			{
				(void)dialogManager.AdvanceDialogue();
			}
		}

		m_registry.ForEach<MovementComponent>(
			[](Entity, MovementComponent& movement)
			{
				movement.velocity = {};
			}
		);
	}
	else
	{
		ControllerSystem::Update(m_registry, input, m_eventBus);
	}

	MovementSystem::Update(m_registry, deltaTime);
	CollisionSystem::Update(m_registry);

	TransformComponent* transform = m_player.Get<TransformComponent>();
	const SpriteComponent* sprite = m_player.Get<SpriteComponent>();

	MapConstraintSystem::ClampToTileMap(*transform, *sprite, m_tileMap);
	AnimationStateSystem::UpdateAnimationStates(m_registry, engine.GetAnimationManager());
	AnimationSystem::Update(m_registry, engine.GetAnimationManager(), deltaTime);
	
	CameraComponent* camera = m_camera.Get<CameraComponent>();
	if (camera == nullptr || transform == nullptr || sprite == nullptr)
	{
		return;
	}

	CameraSystem::UpdateFollow(*camera, *transform, *sprite, engine.GetRenderSystem(), m_tileMap);
}

void PlayState::Render(Engine &engine, SDL_Renderer *renderer)
{
    if (renderer == nullptr)
    {
        return;
    }

    const CameraComponent* camera = m_camera.Get<CameraComponent>();
    if (camera == nullptr)
    {
        return;
    }

    auto& renderSystem = engine.GetRenderSystem();

	auto& mapSystem = engine.GetMapSystem();
	mapSystem.Render(m_tileMap, renderSystem, *camera, m_registry);

    const auto &config = engine.GetConfig();
    auto& dialogManager = engine.GetDialogManager();

    auto& textManager = engine.GetTextManager();
    renderSystem.RenderDialogue(dialogManager.GetRuntimeState(), config.windowWidth, config.windowHeight, textManager);
}

#ifdef _DEBUG
void PlayState::RenderImGui(Engine& engine)
{
	m_debugHelper.RenderImGui(engine, m_registry);
	m_debugHelper.RenderDebugCollider(engine, m_registry, m_camera);
}

#endif