#include "PlayState.h"
#include "../Engine.h"
#include "../ECS/SpriteComponent.h"
#include "../ECS/TransformComponent.h"
#include "../ECS/ControllerComponent.h"
#include "../ECS/MovementComponent.h"
#include "../Systems/AnimationStateSystem.h"
#include "../Systems/ControllerSystem.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/MapConstraintSystem.h"
#include "../Systems/CameraSystem.h"
#include "../Systems/SpriteAnimationSystem.h"
#include <algorithm>

void PlayState::OnEnter(Engine& engine)
{
	engine.GetTextureManager().LoadTexture("player_idle", "Assets/sprites/witch_idle.png");
	engine.GetTextureManager().LoadTexture("player_walk", "Assets/sprites/witch_walk.png");
	engine.GetTextureManager().LoadTexture("player_v2_idle", "Assets/sprites/witch_v2_idle.png");
	engine.GetTextureManager().LoadTexture("player_v2_walk", "Assets/sprites/witch_v2_walk.png");
	engine.GetSoundManager().LoadSound("background_forest_music", "Assets/sounds/forest.ogg");

	m_player = m_registry.CreateEntity(); 
	m_player.Add<TransformComponent>(2000.0f, 480.0f);
	m_player.Add<MovementComponent>();
	m_player.Add<ControllerComponent>();
	auto& sprite = m_player.Add<SpriteComponent>("player_v2_idle", 128, 120);
	auto& playerAnim = m_player.Add<SpriteAnimationComponent>();
	playerAnim.m_animations[AnimationState::Idle] = AnimationClip
	{ 
		.frameWidth = 480, 
		.frameHeight = 500, 
		.frameCount = 5, 
		.frameDuration = 0.16f, 
		.isLooping = true, 
		.textureName = "player_v2_idle",
	};

	playerAnim.m_animations[AnimationState::Walking] = AnimationClip
	{
		.frameWidth = 480,
		.frameHeight = 480,
		.frameCount = 5,
		.frameDuration = 0.16f,
		.row = 3,
		.isLooping = true,
		.textureName = "player_v2_walk",
	};

	AnimationUtils::PlayAnimation(sprite, playerAnim, AnimationState::Idle);

	m_camera = m_registry.CreateEntity();
	m_camera.Add<CameraComponent>(2000, 0, 800, 600);

	m_tileMap.LoadFromTmj("Assets/map/test.tmj", engine.GetTextureManager());

	engine.GetSoundManager().PlaySound("background_forest_music", true);
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

	engine.GetTextureManager().UnloadTexture("player_idle");
	engine.GetTextureManager().UnloadTexture("player_v2_idle");
	m_tileMap.Clear(&engine.GetTextureManager());
}

void PlayState::HandleEvent(Engine& engine, const SDL_Event& event)
{
	(void)engine;
	(void)event;
}

void PlayState::Update(Engine& engine, float deltaTime)
{
	const InputSystem& input = engine.GetInputSystem();

	if (ControllerSystem::PopState(m_registry, input))
	{
		engine.PopState();
		return;
	}

	ControllerSystem::Update(m_registry, input);
	MovementSystem::Update(m_registry, deltaTime);

	CameraComponent* camera = m_camera.Get<CameraComponent>();
	TransformComponent* transform = m_player.Get<TransformComponent>();
	const SpriteComponent* sprite = m_player.Get<SpriteComponent>();

	if (camera == nullptr || transform == nullptr || sprite == nullptr)
	{
		return;
	}

	MapConstraintSystem::ClampToTileMap(*transform, *sprite, m_tileMap);
	CameraSystem::UpdateFollow(*camera, *transform, *sprite, engine.GetRenderSystem(), m_tileMap);
	AnimationStateSystem::UpdateAnimationStates(m_registry);
	SpriteAnimationSystem::Update(m_registry, deltaTime);
}

void PlayState::Render(Engine& engine, SDL_Renderer* renderer)
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

	m_tileMap.Render(engine.GetRenderSystem(), camera->m_viewport, m_registry);
}