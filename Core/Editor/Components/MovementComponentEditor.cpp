#include "MovementComponentEditor.h"
#include "../../ECS/MovementComponent.h"

#ifdef _DEBUG
#include <imgui.h>
#endif

bool MovementComponentEditor::Render(Entity& entity)
{
#ifdef _DEBUG
	MovementComponent* movement = entity.Get<MovementComponent>();

	if (movement == nullptr)
	{
		return false;
	}

	ImGui::SeparatorText("Movement");
	ImGui::PushID("MovementComponent");

	const bool changed =
		ImGui::DragFloat("Move Speed", &movement->moveSpeed, 1.0f, 0.0f, 2000.0f);

	ImGui::PopID();

	return changed;
#else
	(void)entity;
	return false;
#endif
}
