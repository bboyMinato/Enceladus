#include "TagComponentEditor.h"
#include "../../ECS/TagComponent.h"
#include <cstdio>

#ifdef _DEBUG
#include <imgui.h>
#endif

bool TagComponentEditor::Render(Entity& entity)
{
#ifdef _DEBUG
	TagComponent* tag = entity.Get<TagComponent>();

	if (tag == nullptr)
	{
		return false;
	}

	if (m_tagBufferEntity != entity)
	{
		std::snprintf(m_tagBuffer.data(), m_tagBuffer.size(), "%s", tag->Tag.c_str());

		m_tagBufferEntity = entity;
	}

	ImGui::SeparatorText("Tag");
	ImGui::PushID("TagComponent");

	const bool changed = ImGui::InputText("Value", m_tagBuffer.data(), m_tagBuffer.size());

	if (changed)
	{
		tag->Tag = m_tagBuffer.data();
	}

	ImGui::PopID();

	return changed;
#else
	(void)entity;
	return false;
#endif
}
