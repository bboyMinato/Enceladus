#include "PlayState.h"
#include "../Engine.h"
#include "../ECS/SpriteComponent.h"
#include "../ECS/TransformComponent.h"
#include "../ECS/ControllerComponent.h"
#include "../ECS/MovementComponent.h"
#include "../ECS/ColliderComponent.h"
#include "../ECS/TagComponent.h"
#include "../Systems/AnimationStateSystem.h"
#include "../Systems/ControllerSystem.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/MapConstraintSystem.h"
#include "../Systems/CameraSystem.h"
#include "../Systems/SpriteAnimationSystem.h"
#include "../Systems/CollisionSystem.h"
#include <algorithm>

#ifdef _DEBUG
#include <imgui.h>
#include <vector>
#endif

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
	m_testEntity.Add<ColliderComponent>(35, 55, 45, 35);
	m_testEntity.Add<TagComponent>("Test Entity");
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
	m_player.Add<TagComponent>("Player");
	auto& sprite = m_player.Add<SpriteComponent>("player_v2_idle", 128, 120);
	auto& collider = m_player.Add<ColliderComponent>(35, 55, 45, 35);
	auto& playerAnim = m_player.Add<SpriteAnimationComponent>();
	playerAnim.m_animations[AnimationState::Idle] = AnimationClip
	{ 
		.frameWidth = 480, 
		.frameHeight = 480, 
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

	m_camera = m_registry.CreateEntity();
	m_camera.Add<CameraComponent>(2000, 0, 800, 600);
	m_camera.Add<TagComponent>("Main Camera");

	m_tileMap.LoadFromTmj("Assets/map/test.tmj", engine.GetTextureManager());

	engine.GetSoundManager().PlaySound("background_forest_music", true);

#ifdef _DEBUG
	m_showDebugWindow = true;
	m_showColliderDebug = true;
	m_selectedDebugEntityId = m_player.GetId();
#endif
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

#ifdef _DEBUG
	if (!m_showColliderDebug)
	{
		return;
	}

	for (auto& entity : m_registry.GetEntitiesWithComponents<ColliderComponent, TransformComponent>())
	{
		const auto& collider = entity.Get<ColliderComponent>();
		const auto& transform = entity.Get<TransformComponent>();

		SDL_Rect colliderRect = SDL_Rect
		{
			.x = static_cast<int>(transform->x + collider->m_offsetX - camera->m_viewport.x),
			.y = static_cast<int>(transform->y + collider->m_offsetY - camera->m_viewport.y),
			.w = collider->m_width,
			.h = collider->m_height
		};

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

		if (m_selectedDebugEntityId == entity.GetId())
		{
			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		}

		SDL_RenderDrawRect(renderer, &colliderRect);
	}
#endif
}

#ifdef _DEBUG
void PlayState::RenderImGui(Engine& engine)
{
	(void)engine;

	if (!m_showDebugWindow)
	{
		return;
	}

	ImGui::Begin("PlayState Debug", &m_showDebugWindow);

	ImGui::Checkbox("Show Collider Debug", &m_showColliderDebug);
		
	std::vector<Entity> entities = m_registry.GetAllEntities();

	Entity selectedEntity;
	for (const Entity& entity : entities)
	{
		if (entity.GetId() == m_selectedDebugEntityId)
		{
			selectedEntity = entity;
			break;
		}
	}

	std::string selectedTag = selectedEntity.IsValid() && selectedEntity.HasComponent<TagComponent>()
		? selectedEntity.Get<TagComponent>()->Tag
		: "Unknown";

	std::string selectedLabel = selectedEntity.IsValid()
		? ("Entity [" + std::to_string(selectedEntity.GetId()) + "] - " + selectedTag)
		: "None";

	if (ImGui::BeginCombo("Selected Entity", selectedLabel.c_str()))
	{
		for (const Entity& entity : entities)
		{
			const std::string tag = entity.HasComponent<TagComponent>() ? entity.Get<TagComponent>()->Tag : "Unknown";
			const std::string label = "Entity [" + std::to_string(entity.GetId()) + "] - " + tag;
			const bool isSelected = (entity.GetId() == m_selectedDebugEntityId);

			if (ImGui::Selectable(label.c_str(), isSelected))
			{
				m_selectedDebugEntityId = entity.GetId();
			}

			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}
	
	if (ImGui::BeginTabBar("PlayStateDebugTabs"))
	{
		if (ImGui::BeginTabItem("Entity"))
		{
			RenderEntityTab(selectedEntity);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Collider"))
		{
			RenderColliderTab(selectedEntity);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Animation"))
		{
			RenderAnimationTab(selectedEntity);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Camera"))
		{
			RenderCameraTab(m_camera);
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void PlayState::RenderEntityTab(Entity selectedEntity)
{
	TransformComponent* transform = selectedEntity.Get<TransformComponent>();
	MovementComponent* movement = selectedEntity.Get<MovementComponent>();
	TagComponent* tag = selectedEntity.Get<TagComponent>();

	ImGui::Text("Entity: %s", tag ? tag->Tag.c_str() : "Unknown");
	ImGui::Text("Entity Id: %u", selectedEntity.GetId());

	if (transform != nullptr)
	{
		ImGui::SeparatorText("Transform");
		ImGui::Text("Position: (%.2f, %.2f)", transform->x, transform->y);
		ImGui::DragFloat2("Edit Position", &transform->x, 2.0f);
	}

	if (movement != nullptr)
	{
		ImGui::SeparatorText("Movement");
		ImGui::Checkbox("Manual Movement Override", &movement->m_useManualMovement);

		ImGui::Text("Velocity: (%.2f, %.2f)", movement->velocity.x, movement->velocity.y);

		if (!movement->m_useManualMovement)
		{
			ImGui::BeginDisabled();
		}

		ImGui::DragFloat2("Edit Velocity", &movement->velocity.x, 0.05f, -1.0f, 1.0f);

		if (!movement->m_useManualMovement)
		{
			ImGui::EndDisabled();
		}

		ImGui::DragFloat("Move Speed", &movement->moveSpeed, 1.0f, 0.0f, 1000.0f);

		movement->moveSpeed = std::max(movement->moveSpeed, 0.0f);

		if (!movement->m_useManualMovement)
		{
			ImGui::TextDisabled("Enable manual override to edit velocity.");
		}
	}
}

void PlayState::RenderColliderTab(Entity selectedEntity)
{
	ColliderComponent* collider = selectedEntity.Get<ColliderComponent>();

	if (collider != nullptr)
	{
		ImGui::DragInt("Collider Width", &collider->m_width, 1.0f, 1, 2000);
		ImGui::DragInt("Collider Height", &collider->m_height, 1.0f, 1, 2000);
		ImGui::DragInt("Collider Offset X", &collider->m_offsetX, 1.0f, -1000, 1000);
		ImGui::DragInt("Collider Offset Y", &collider->m_offsetY, 1.0f, -1000, 1000);

		collider->m_width = std::max(collider->m_width, 1);
		collider->m_height = std::max(collider->m_height, 1);

		ImGui::Checkbox("Is Trigger", &collider->m_isTrigger);
		ImGui::Checkbox("Is Static", &collider->m_isStatic);
	}
	else
	{
		ImGui::TextDisabled("No ColliderComponent");
	}
}

void PlayState::RenderAnimationTab(Entity selectedEntity)
{
	SpriteComponent* sprite = selectedEntity.Get<SpriteComponent>();
	SpriteAnimationComponent* animation = selectedEntity.Get<SpriteAnimationComponent>();

	if (animation != nullptr)
	{
		ImGui::Checkbox("Manual Animation Override", &animation->m_useManualState);
		ImGui::Checkbox("Is Playing", &animation->m_isPlaying);
		ImGui::Text("Current Frame: %d", animation->m_currentFrame);
		ImGui::Text("Elapsed Time: %.3f", animation->m_elapsedTime);

		struct AnimationOption
		{
			AnimationState state;
			const char* label;
		};

		static constexpr AnimationOption allAnimationOptions[]
		{
			{ AnimationState::Idle, "Idle" },
			{ AnimationState::Walking, "Walking" },
			{ AnimationState::Running, "Running" },
			{ AnimationState::Jumping, "Jumping" },
			{ AnimationState::Falling, "Falling" },
			{ AnimationState::Attacking, "Attacking" },
			{ AnimationState::Dying, "Dying" }
		};

		std::vector<AnimationOption> availableAnimations;
		availableAnimations.reserve(IM_ARRAYSIZE(allAnimationOptions));

		int currentAnimationIndex = -1;

		for (const AnimationOption& option : allAnimationOptions)
		{
			if (animation->m_animations.find(option.state) == animation->m_animations.end())
			{
				continue;
			}

			if (option.state == animation->m_currentState)
			{
				currentAnimationIndex = static_cast<int>(availableAnimations.size());
			}

			availableAnimations.push_back(option);
		}

		if (!availableAnimations.empty())
		{
			std::vector<const char*> animationLabels;
			animationLabels.reserve(availableAnimations.size());

			for (const AnimationOption& option : availableAnimations)
			{
				animationLabels.push_back(option.label);
			}

			if (currentAnimationIndex < 0)
			{
				currentAnimationIndex = 0;
			}

			const bool canEditState = animation->m_useManualState;

			if (!canEditState)
			{
				ImGui::BeginDisabled();
			}

			if (ImGui::Combo("Current State", &currentAnimationIndex, animationLabels.data(), static_cast<int>(animationLabels.size())))
			{
				const AnimationState newState = availableAnimations[currentAnimationIndex].state;

				if (sprite != nullptr)
				{
					AnimationUtils::PlayAnimation(*sprite, *animation, newState);
				}
			}

			if (!canEditState)
			{
				ImGui::EndDisabled();
			}
		}
		else
		{
			ImGui::TextDisabled("No animation clips available");
		}

		auto clipIt = animation->m_animations.find(animation->m_currentState);
		if (clipIt != animation->m_animations.end())
		{
			AnimationClip& clip = clipIt->second;

			ImGui::SeparatorText("Current Clip");
			ImGui::Text("Texture: %s", clip.textureName.c_str());
			ImGui::DragInt("Frame Width", &clip.frameWidth, 1.0f, 1, 4096);
			ImGui::DragInt("Frame Height", &clip.frameHeight, 1.0f, 1, 4096);
			ImGui::DragInt("Frame Count", &clip.frameCount, 1.0f, 1, 256);
			ImGui::DragFloat("Frame Duration", &clip.frameDuration, 0.01f, 0.01f, 10.0f, "%.2f");
			ImGui::DragInt("Row", &clip.row, 1.0f, 0, 128);
			ImGui::DragInt("Start Frame", &clip.startFrame, 1.0f, 0, 256);
			ImGui::Checkbox("Looping", &clip.isLooping);

			clip.frameWidth = std::max(clip.frameWidth, 1);
			clip.frameHeight = std::max(clip.frameHeight, 1);
			clip.frameCount = std::max(clip.frameCount, 1);
			clip.frameDuration = std::max(clip.frameDuration, 0.01f);

			if (ImGui::Button("Restart Animation"))
			{
				animation->m_currentFrame = 0;
				animation->m_elapsedTime = 0.0f;
				animation->m_isPlaying = true;

				if (sprite != nullptr)
				{
					sprite->m_textureName = clip.textureName;
				}
			}
		}
	}
	else
	{
		ImGui::TextDisabled("No SpriteAnimationComponent");
	}
}

void PlayState::RenderCameraTab(Entity cameraEntity)
{
	CameraComponent* camera = m_camera.Get<CameraComponent>();

	if (camera != nullptr)
	{
		ImGui::Text("Viewport Position: (%d, %d)", camera->m_viewport.x, camera->m_viewport.y);
		ImGui::Text("Viewport Size: %d x %d", camera->m_viewport.w, camera->m_viewport.h);
	}
	else
	{
		ImGui::TextDisabled("No CameraComponent");
	}
}
#endif