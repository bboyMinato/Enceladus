#pragma once
#include "../../World/Scene.h"
#include "../EditorContext.h"
#include <array>
#include <string>

class SceneHierarchyPanel
{
public:
	void Render(Scene& scene, EditorContext& context);

	bool DeleteSelectedEntity(Scene& scene, EditorContext& context);
private:
	[[nodiscard]]
	std::string GetEntityLabel(const Scene& scene, Entity entity) const;

	bool RenderEntity(Scene& scene, Entity entity, EditorContext& context);
	
	void BeginRename(Scene& scene, Entity entity);
	void RenderRenamePopup(Scene& scene, EditorContext& context);

private:
	Entity m_entityBeingRenamed;
	std::array<char, 128> m_renameBuffer{};
	bool m_isRenamePopupOpen{ false };
};