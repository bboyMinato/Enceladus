#include "PlayState.h"
#include "../Engine.h"
#include "../ECS/SpriteComponent.h"
#include "../ECS/TransformComponent.h"
#include "../ECS/ControllerComponent.h"
#include "../ECS/MovementComponent.h"
#include "../ECS/ColliderComponent.h"
#include "../Systems/AnimationStateSystem.h"
#include "../Systems/ControllerSystem.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/MapConstraintSystem.h"
#include "../Systems/CameraSystem.h"
#include "../Systems/SpriteAnimationSystem.h"
#include "../Systems/CollisionSystem.h"
#include <algorithm>

void PlayState::OnEnter(Engine& engine)
{
	engine.GetTextureManager().LoadTexture("player_idle", "Assets/sprites/witch_idle.png");
	engine.GetTextureManager().LoadTexture("player_walk", "Assets/sprites/witch_walk.png");
	engine.GetTextureManager().LoadTexture("player_v2_idle", "Assets/sprites/witch_v2_idle.png");
	engine.GetTextureManager().LoadTexture("player_v2_walk", "Assets/sprites/witch_v2_walk.png");
	engine.GetSoundManager().LoadSound("background_forest_music", "Assets/sounds/forest.ogg");

	m_testEntity = m_registry.CreateEntity();
	m_testEntity.Add<TransformComponent>(1500.0f, 500.0f);
	m_testEntity.Add<SpriteComponent>("player_idle", 128, 120);
	m_testEntity.Add<ColliderComponent>(128, 120, 0, 0);
	auto& testAnim = m_testEntity.Add<SpriteAnimationComponent>();
	testAnim.m_animations[AnimationState::Idle] = AnimationClip
	{
		.frameWidth = 480,
		.frameHeight = 500,
		.frameCount = 5,
		.frameDuration = 0.16f,
		.isLooping = true,
		.textureName = "player_idle",
	};

	m_player = m_registry.CreateEntity(); 
	m_player.Add<TransformComponent>(2000.0f, 480.0f);
	m_player.Add<MovementComponent>();
	m_player.Add<ControllerComponent>();
	auto& sprite = m_player.Add<SpriteComponent>("player_v2_idle", 128, 120);
	auto& collider = m_player.Add<ColliderComponent>(128, 120, 0, 0);
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
	engine.GetTextureManager().UnloadTexture("player_walk");
	engine.GetTextureManager().UnloadTexture("player_v2_walk");
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
	CollisionSystem::Update(m_registry);

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

	const auto& collider = m_player.Get<ColliderComponent>();
	const auto& transform = m_player.Get<TransformComponent>();

	SDL_Rect colliderRect = SDL_Rect
	{
		.x = static_cast<int>(transform->x + collider->m_offsetX - camera->m_viewport.x),
		.y = static_cast<int>(transform->y + collider->m_offsetY - camera->m_viewport.y),
		.w = collider->m_width,
		.h = collider->m_height
	};

	const auto& testCollider = m_testEntity.Get<ColliderComponent>();
	const auto& testTransform = m_testEntity.Get<TransformComponent>();

	SDL_Rect testColliderRect = SDL_Rect
	{
		.x = static_cast<int>(testTransform->x + testCollider->m_offsetX - camera->m_viewport.x),
		.y = static_cast<int>(testTransform->y + testCollider->m_offsetY - camera->m_viewport.y),
		.w = testCollider->m_width,
		.h = testCollider->m_height
	};

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderDrawRect(renderer, &colliderRect);
	SDL_RenderDrawRect(renderer, &testColliderRect);
}