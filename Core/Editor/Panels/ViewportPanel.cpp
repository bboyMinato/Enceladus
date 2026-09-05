#include "ViewportPanel.h"

#ifdef _DEBUG
#include <imgui.h>
#endif

void ViewportPanel::Render(Engine& engine, Scene& scene, EditorContext& context)
{
	if (ImGui::Begin("Viewport"))
	{
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		ImGui::Image((void*)0, viewportSize);
	}

	ImGui::End();
}