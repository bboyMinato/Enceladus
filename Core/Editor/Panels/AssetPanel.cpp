#include "AssetPanel.h"

#ifdef _DEBUG
#include <imgui.h>
#endif

void AssetPanel::Render()
{
#ifdef _DEBUG
	if (ImGui::Begin("Assets"))
	{
		ImGui::TextUnformatted("Asset browser will be implemented in Phase 6.");
	}

	ImGui::End();
#endif
}
