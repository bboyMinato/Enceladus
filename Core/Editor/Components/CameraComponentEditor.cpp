#include "CameraComponentEditor.h"
#include "../../ECS/CameraComponent.h"
#include <algorithm>

#ifdef _DEBUG
#include <imgui.h>
#endif

bool CameraComponentEditor::Render(Entity& entity)
{
#ifdef _DEBUG
	CameraComponent* camera = entity.Get<CameraComponent>();

	if (camera == nullptr)
	{
		return false;
	}

	ImGui::SeparatorText("Camera");
	ImGui::PushID("CameraComponent");

	bool changed = false;

	constexpr const char* cameraModes[]
	{
		"Follow",
		"Free",
		"Fixed"
	};

	int modeIndex = static_cast<int>(camera->m_mode);

	if (ImGui::Combo("Mode", &modeIndex, cameraModes, static_cast<int>(std::size(cameraModes))))
	{
		camera->m_mode = static_cast<CameraMode>(modeIndex);
		changed = true;
	}

	changed |= ImGui::DragFloat("Zoom", &camera->m_zoom, 0.01f, 0.1f, 10.0f);

	changed |= ImGui::Checkbox("Active", &camera->m_isActive);
	changed |= ImGui::Checkbox("Should Follow", &camera->m_shouldFollow);
	changed |= ImGui::DragFloat2("Follow Offset", &camera->m_followOffset.x, 1.0f); 

	changed |= ImGui::DragInt4("Viewport", &camera->m_viewport.x, 1.0f);

	camera->m_viewport.w = std::max(camera->m_viewport.w, 1);
	camera->m_viewport.h = std::max(camera->m_viewport.h, 1);

	ImGui::PopID();

	return changed;
#else
	(void)entity;
	return false;
#endif
}