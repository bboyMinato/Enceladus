#include "DialogSystem.h"

#include <iostream>

#include "InputSystem.h"
#include "RenderSystem.h"
#include "../ECS/ControllerComponent.h"
#include "../ECS/DialogInitComponent.h"
#include "../ECS/TransformComponent.h"
#include "../ECS/DialogSegmentComponent.h"
#include "../Managers/DialogManager.h"
#include "../Math/Vector2.h"

void DialogSystem::Update(const Entity& player, const InputSystem& input, Registry& registry, const DialogManager& dialogManager)
{
	auto controller = player.Get<ControllerComponent>();

	for (const auto& dialogEntity : registry.GetEntitiesWithComponents<DialogSegmentComponent>()) {
		if (input.IsKeyDown(controller->confirmKey))

		return;
	}
	
	auto transform = player.Get<TransformComponent>();
	auto vector = Vector2f{ transform->x, transform->y };

	if (input.IsKeyDown(controller->confirmKey))
	{
		for (const auto& otherEntity : registry.GetEntitiesWithComponents<TransformComponent, DialogInitComponent>())
		{
			auto otherTransform = otherEntity.Get<TransformComponent>();
			auto otherVector = Vector2f{ otherTransform->x, otherTransform->y };
			auto otherDialog = otherEntity.Get<DialogInitComponent>();

			auto diff = otherVector - vector;

			if (diff.Length() < 64.0f)
			{
				auto segmentEntity = registry.CreateEntity();
				auto dialogSegmentComponent = registry.AddComponent<DialogSegmentComponent>(segmentEntity, otherDialog->name);
				UpdateText(dialogSegmentComponent, dialogManager);
			}
		}

	}
}

void DialogSystem::UpdateText(DialogSegmentComponent& segmentComponent, const DialogManager& dialogManager)
{
	const auto& dialog = dialogManager.GetDialog(segmentComponent.name);
	const auto& entries = dialog.GetEntries();
	
	segmentComponent.text = entries.front().GetSpeech();
}