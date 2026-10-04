#include "../../ECS/SpriteAnimationComponent.h"
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

				// A manually selected texture must not be overwritten by an animation set.
				if (entity.HasComponent<SpriteAnimationComponent>())
				{
					entity.Remove<SpriteAnimationComponent>();
				}

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

	if (ImGui::Checkbox("Use Source Rectangle", &sprite->m_hasSourceRect))
	{
		if (sprite->m_hasSourceRect &&
			(sprite->m_sourceRect.w <= 0 || sprite->m_sourceRect.h <= 0))
		{
			// Initial editable frame size only; adjust these to the actual sheet frame size.
			sprite->m_sourceRect = SDL_Rect
			{
				.x = 0,
				.y = 0,
				.w = std::max(sprite->m_width, 1),
				.h = std::max(sprite->m_height, 1)
			};
		}

		changed = true;
	}

	if (sprite->m_hasSourceRect)
	{
		changed |= ImGui::DragInt("Frame X", &sprite->m_sourceRect.x, 1.0f, 0, 4096);
		changed |= ImGui::DragInt("Frame Y", &sprite->m_sourceRect.y, 1.0f, 0, 4096);
		changed |= ImGui::DragInt("Frame Width", &sprite->m_sourceRect.w, 1.0f, 1, 4096);
		changed |= ImGui::DragInt("Frame Height", &sprite->m_sourceRect.h, 1.0f, 1, 4096);
	}

	ImGui::PopID();

	return changed;
#else
	(void)entity;
	(void)scene;
	return false;
#endif
}