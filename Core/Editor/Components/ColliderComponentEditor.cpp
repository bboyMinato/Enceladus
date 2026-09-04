#include "ColliderComponentEditor.h"
#include "../../ECS/ColliderComponent.h"
#include <algorithm>

#ifdef _DEBUG
#include <imgui.h>
#endif

bool ColliderComponentEditor::Render(Entity& entity)
{
#ifdef _DEBUG
	ColliderComponent* collider = entity.Get<ColliderComponent>();

	if (collider == nullptr)
	{
		return false;
	}

	ImGui::SeparatorText("Collider");
	ImGui::PushID("ColliderComponent");

	bool changed = false;
	changed |= ImGui::DragInt("Width", &collider->m_width, 1.0f, 1, 4096);
	changed |= ImGui::DragInt("Height", &collider->m_height, 1.0f, 1, 4096);
	changed |= ImGui::DragInt("Offset X", &collider->m_offsetX, 1.0f, -4096, 4096);
	changed |= ImGui::DragInt("Offset Y", &collider->m_offsetY, 1.0f, -4096, 4096);
	changed |= ImGui::Checkbox("Is Trigger", &collider->m_isTrigger);
	changed |= ImGui::Checkbox("Is Static", &collider->m_isStatic);

	collider->m_width = std::max(collider->m_width, 1);
	collider->m_height = std::max(collider->m_height, 1);

	ImGui::PopID();

	return changed;
#else
	(void)entity;
	return false;
#endif
}