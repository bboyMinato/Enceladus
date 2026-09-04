#include "../../ECS/CameraComponent.h"
#include "../../ECS/ColliderComponent.h"
#include "../../ECS/MovementComponent.h"
#include "../../ECS/SpriteAnimationComponent.h"
#include "../../ECS/SpriteComponent.h"
#include "../../ECS/TagComponent.h"
#include "../../ECS/TransformComponent.h"
#include "../Components/CameraComponentEditor.h"
#include "../Components/ColliderComponentEditor.h"
#include "../Components/MovementComponentEditor.h"
#include "../Components/SpriteAnimationComponentEditor.h"
#include "../Components/SpriteComponentEditor.h"
#include "../Components/TransformComponentEditor.h"
#include "InspectorPanel.h"

#ifdef _DEBUG
#include <imgui.h>
#endif

void InspectorPanel::Render(Engine& engine, Scene& scene, EditorContext& context)
{
	(void)engine;

#ifdef _DEBUG
	context.ValidateSelection(scene.GetRegistry());

	if (!ImGui::Begin("Inspector"))
	{
		ImGui::End();

		return;
	}

	Entity selectedEntity = context.GetSelectedEntity();

	if (!selectedEntity)
	{
		ImGui::TextUnformatted("No entity selected.");
		ImGui::End();

		return;
	}

	ImGui::Text("Entity ID: %u", selectedEntity.GetId());

	const std::string_view key = scene.GetEntityKey(selectedEntity);
	if (!key.empty())
	{
		ImGui::Text("Key: %s", key.data());
	}

	ImGui::Separator();

	bool changed = false;
	changed |= RenderEntity(engine, selectedEntity, scene);
	changed |= RenderAddComponent(engine, scene, selectedEntity);

	if (changed)
	{
		context.MarkDirty();
	}

	ImGui::End();
#else
	(void)scene;
	(void)context;
#endif
}

bool InspectorPanel::RenderEntity(Engine& engine, Entity& entity, const Scene& scene)
{
#ifdef _DEBUG
	bool changed = false;

	if (entity.HasComponent<TransformComponent>())
	{
		changed |= TransformComponentEditor::Render(entity);
	}

	if (entity.HasComponent<SpriteComponent>())
	{
		changed |= SpriteComponentEditor::Render(entity, scene);
	}

	if (entity.HasComponent<ColliderComponent>())
	{
		changed |= ColliderComponentEditor::Render(entity);
	}

	if (entity.HasComponent<TagComponent>())
	{
		changed |= m_tagComponentEditor.Render(entity);
	}

	if (entity.HasComponent<MovementComponent>())
	{
		changed |= MovementComponentEditor::Render(entity);
	}

	if (entity.HasComponent<CameraComponent>())
	{
		changed |= CameraComponentEditor::Render(entity);
	}

	if (entity.HasComponent<SpriteAnimationComponent>())
	{
		changed |= SpriteAnimationComponentEditor::Render(engine, entity);
	}

	return changed;
#else
	(void)engine;
	(void)entity;
	(void)scene;
	return false;
#endif
}

bool InspectorPanel::RenderAddComponent(Engine& engine, Scene& scene, Entity& entity)
{
#ifdef _DEBUG
	bool changed = false;

	if (ImGui::Button("Add Component"))
	{
		ImGui::OpenPopup("Add Component");
	}

	if (!ImGui::BeginPopup("Add Component"))
	{
		return false;
	}

	if (ImGui::MenuItem("Transform", nullptr, false, !entity.HasComponent<TransformComponent>()))
	{
		entity.Add<TransformComponent>();
		changed = true;
	}

	if (ImGui::MenuItem("Sprite", nullptr, false, !entity.HasComponent<SpriteComponent>() && !scene.GetTextures().empty()))
	{
		entity.Add<SpriteComponent>(scene.GetTextures().front(), 32, 32);
		changed = true;
	}

	if (ImGui::MenuItem("Collider", nullptr, false, !entity.HasComponent<ColliderComponent>()))
	{
		entity.Add<ColliderComponent>(32, 32);
		changed = true;
	}

	if (ImGui::MenuItem("Tag", nullptr, false, !entity.HasComponent<TagComponent>()))
	{
		entity.Add<TagComponent>("New Tag");
		changed = true;
	}

	if (ImGui::MenuItem("Movement", nullptr, false, !entity.HasComponent<MovementComponent>()))
	{
		entity.Add<MovementComponent>();
		changed = true;
	}

	if (ImGui::MenuItem("Camera", nullptr, false, !entity.HasComponent<CameraComponent>()))
	{
		entity.Add<CameraComponent>();
		changed = true;
	}

	const auto& animationSets = engine.GetAnimationManager().GetAnimationSets();

	if (ImGui::MenuItem("Sprite Animation", nullptr, false, !entity.HasComponent<SpriteAnimationComponent>() && !animationSets.empty()))
	{
		auto& animation = entity.Add<SpriteAnimationComponent>();
		animation.animationSetName = animationSets.begin()->first;
		changed = true;
	}

	ImGui::EndPopup();

	return changed;
#else
	(void)engine;
	(void)scene;
	(void)entity;
	return false;
#endif
}