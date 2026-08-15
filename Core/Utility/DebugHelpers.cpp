#ifdef  _DEBUG

#include "DebugHelpers.h"
#include <imgui.h>
#include "../ECS/TagComponent.h"
#include "../ECS/TransformComponent.h"
#include "../ECS/MovementComponent.h"
#include "../ECS/ColliderComponent.h"
#include "../ECS/SpriteComponent.h"
#include "../ECS/InteractableComponent.h"

void DebugHelper::RenderImGui(Engine& engine, Registry& registry)
{
	if (!m_showDebugWindow)
	{
		return;
	}

	ImGui::Begin("Engine Debug", &m_showDebugWindow);

	std::vector<Entity> entities = registry.GetAllEntities();

	Entity selectedEntity;

	for (const Entity& entity : entities)
	{
		if (entity.GetId() == m_selectedEntityId)
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
			const bool isSelected = (entity.GetId() == m_selectedEntityId);

			if (ImGui::Selectable(label.c_str(), isSelected))
			{
				m_selectedEntityId = entity.GetId();
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

		if (ImGui::BeginTabItem("Sprite"))
		{
			RenderSpriteTab(selectedEntity);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Animation"))
		{
			RenderAnimationTab(selectedEntity, engine);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Interaction"))
		{
			RenderInteractionTab(selectedEntity);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Camera"))
		{
			RenderCameraTab(selectedEntity);
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void DebugHelper::RenderDebugCollider(Engine& engine, Registry& registry, Entity& camera)
{
	auto& renderSystem = engine.GetRenderSystem();

	renderSystem.RenderDebugCollider(registry, *camera.Get<CameraComponent>(), m_selectedEntityId, m_showColliderDebug);
}

void DebugHelper::RenderEntityTab(Entity& selectedEntity)
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

void DebugHelper::RenderColliderTab(Entity& selectedEntity)
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

void DebugHelper::RenderSpriteTab(Entity& selectedEntity)
{
	auto* sprite = selectedEntity.Get<SpriteComponent>();

	if (sprite == nullptr)
	{
		ImGui::TextDisabled("No SpriteComponent");

		return;
	}

	ImGui::SeparatorText("Sprite");
	ImGui::Text("Texture name: %s", sprite->m_textureName.c_str());
	ImGui::Checkbox("Visible", &sprite->m_isVisible);
}

void DebugHelper::RenderAnimationTab(Entity& selectedEntity, Engine& engine)
{
	auto* animDef = selectedEntity.Get<SpriteAnimationComponent>();

	if (animDef == nullptr)
	{
		ImGui::TextDisabled("No SpriteAnimationComponent");

		return;
	}

	auto& animationManager = engine.GetAnimationManager();

	auto& animationSets = animationManager.GetAnimationSets();
	std::vector<const char*> animationSetNames;

	for (const auto& pair : animationSets)
	{
		animationSetNames.push_back(pair.first.c_str());
	}

	ImGui::SeparatorText("Animation");

	ImGui::Checkbox("Manual Animation Override", &animDef->m_useManualState);

	static int currentAnimationSetIndex = 0;

	if (ImGui::Combo("AnimationSet", &currentAnimationSetIndex, animationSetNames.data(), static_cast<int>(animationSetNames.size())))
	{
		animDef->animationSetName = animationSetNames[currentAnimationSetIndex];
	}

	auto& animations = animationManager.GetAnimations(animDef->animationSetName);
	std::vector<const char*> animationNames;

	for (const auto& pair : animations)
	{
		animationNames.push_back(pair.first.c_str());
	}

	static int currentAnimationIndex = 0;
	if (ImGui::Combo("Animations", &currentAnimationIndex, animationNames.data(), static_cast<int>(animationNames.size())))
	{
		animDef->currentAnimation = animationNames[currentAnimationIndex];
	}

	ImGui::Text("Current animation: %s", animDef->currentAnimation.c_str());
	ImGui::DragInt("Current Frame", &animDef->currentFrame, 1.0f, 0, 1000);
}

void DebugHelper::RenderCameraTab(Entity& cameraEntity)
{
	CameraComponent* camera = cameraEntity.Get<CameraComponent>();

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

void DebugHelper::RenderInteractionTab(Entity& selectedEntity)
{
	InteractableComponent* interactable = selectedEntity.Get<InteractableComponent>();

	if (interactable != nullptr)
	{
		const char* interactionTypes[] = { "Dialogue", "Open", "Activate", "Pickup", "Examine", "Default" };
		int currentTypeIndex = static_cast<int>(interactable->interactionType);
		if (ImGui::Combo("Interaction Type", &currentTypeIndex, interactionTypes, IM_ARRAYSIZE(interactionTypes)))
		{
			interactable->interactionType = static_cast<InteractionType>(currentTypeIndex);
		}
		ImGui::DragFloat("Interaction Distance", &interactable->interactionDistance, 1.0f, 0.0f, 1000.0f);
		ImGui::Checkbox("Requires Key", &interactable->requiresKey);
		ImGui::Checkbox("One Shot", &interactable->oneShot);
		ImGui::Checkbox("Used", &interactable->used);
		ImGui::InputText("Dialogue ID", &interactable->dialogueId[0], interactable->dialogueId.size() + 1);
	}
	else
	{
		ImGui::TextDisabled("No Interactable Component");
	}
}

#endif 