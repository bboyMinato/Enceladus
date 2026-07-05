#include "DialogSystem.h"
#include "InputSystem.h"
#include "RenderSystem.h"
#include "../ECS/ControllerComponent.h"
#include "../ECS/DialogInitComponent.h"
#include "../ECS/TransformComponent.h"
#include "../ECS/DialogSegmentComponent.h"
#include "../Managers/DialogManager.h"
#include "../Math/Vector2.h"

DialogSystemState DialogSystem::m_state = DialogSystemState::Hide;

void DialogSystem::Update(const Entity& player, const InputSystem& input, Registry& registry,
                          const DialogManager& dialogManager)
{
	auto controller = player.Get<ControllerComponent>();
	if (input.IsKeyDown(controller->confirmKey))
	{
		switch (m_state)
		{
		case DialogSystemState::Hide:
			InitDialog(player, registry, dialogManager);
			break;

		case DialogSystemState::Show:
			AdvanceDialog(registry, dialogManager);
			break;
		}
	}
}

void DialogSystem::AdvanceDialog(Registry& registry, const DialogManager& dialogManager)
{
	for (const auto& dialogEntity : registry.GetEntitiesWithComponents<DialogSegmentComponent>())
	{
		auto& segmentComponent = *registry.GetComponent<DialogSegmentComponent>(dialogEntity);
		segmentComponent.part++;

		if (UpdateText(segmentComponent, dialogManager) != UpdateTextResult::Exit)
		{
			m_state = DialogSystemState::Hide;
		}

		return;
	}
}

void DialogSystem::InitDialog(const Entity& player, Registry& registry, const DialogManager& dialogManager)
{
	auto transform = player.Get<TransformComponent>();
	auto vector = Vector2f{ transform->x, transform->y };

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
			if (UpdateText(dialogSegmentComponent, dialogManager) != UpdateTextResult::Exit)
			{
				m_state = DialogSystemState::Show;
			}
		}
	}
}

UpdateTextResult DialogSystem::UpdateText(DialogSegmentComponent& segmentComponent, const DialogManager& dialogManager)
{
	const auto& dialog = dialogManager.GetDialog(segmentComponent.name);
	const auto& entries = dialog.GetEntries();

	const auto& part = segmentComponent.part;
	if (part >= entries.size())
	{
		return UpdateTextResult::Exit;
	}

	const auto& speech = entries[part].GetSpeech();
	const auto& pos = segmentComponent.pos;
	if (pos >= speech.length())
	{
		return UpdateTextResult::Waiting;
	}

	segmentComponent.text = entries[part].GetSpeech().substr(0, pos);
	return UpdateTextResult::Advanced;
}
