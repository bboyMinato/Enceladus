#include "../../ECS/SpriteComponent.h"
#include "../../World/Scene.h"
#include "SpriteComponentEditor.h"
#include <algorithm>

#ifdef _DEBUG
#include <imgui.h>
#endif

bool SpriteComponentEditor::Render(Entity& entity, const Scene& scene)
{
#ifdef _DEBUG
	SpriteComponent* sprite = entity.Get<SpriteComponent>();

	if (sprite == nullptr)
	{
		return false;
	}

	ImGui::SeparatorText("Sprite");
	ImGui::PushID("SpriteComponent");

	bool changed = false;

	if (ImGui::BeginCombo("Texture", sprite->m_textureName.c_str()))
	{
		for (const std::string& textureName : scene.GetTextures())
		{
			const bool isSelected = textureName == sprite->m_textureName;

			if (ImGui::Selectable(textureName.c_str(), isSelected))
			{
				sprite->m_textureName = textureName;
				changed = true;
			}

			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}

	changed |= ImGui::DragInt("Width", &sprite->m_width, 1.0f, 1, 4096);
	changed |= ImGui::DragInt("Height", &sprite->m_height, 1.0f, 1, 4096);
	changed |= ImGui::Checkbox("Visible", &sprite->m_isVisible);
	changed |= ImGui::Checkbox("Use Source Rectangle", &sprite->m_hasSourceRect);

	if (sprite->m_hasSourceRect)
	{
		changed |= ImGui::DragInt4("Source Rectangle", &sprite->m_sourceRect.x, 1.0f);

		sprite->m_sourceRect.w = std::max(sprite->m_sourceRect.w, 1);
		sprite->m_sourceRect.h = std::max(sprite->m_sourceRect.h, 1);
	}

	ImGui::PopID();

	return changed;
#else
	(void)entity;
	(void)scene;
	return false;
#endif
}