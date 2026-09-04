#include "../../ECS/TransformComponent.h"
#include "TransformComponentEditor.h"

#ifdef _DEBUG
#include <imgui.h>  
#endif // DEBUG


bool TransformComponentEditor::Render(Entity& entity)
{
#ifdef _DEBUG
	TransformComponent* transform = entity.Get<TransformComponent>();

	if (transform == nullptr)
	{
		return false;
	}

	ImGui::SeparatorText("Transform");
	ImGui::PushID("TransformComponent");

	bool changed = false;
	changed |= ImGui::DragFloat2("Position", &transform->x, 1.0f);
	changed |= ImGui::DragFloat("Rotation", &transform->rotation, 1.0f);
	changed |= ImGui::DragFloat2("Scale", &transform->scaleX, 0.01f);

	ImGui::PopID();

	return changed;
#else
	(void)entity;
	return false;
#endif
}