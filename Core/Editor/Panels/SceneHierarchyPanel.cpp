#include "SceneHierarchyPanel.h"
#include "../../ECS/TagComponent.h"

#ifdef _DEBUG
#include <imgui.h>
#endif

void SceneHierarchyPanel::Render(Scene& scene, EditorContext& context)
{
#ifdef _DEBUG
	Entity entityToDelete;

	if (ImGui::Begin("Scene Hierarchy"))
	{
		if (ImGui::Button("Create Empty Entity"))
		{
			Entity entity = scene.CreateEntity();

			if (entity)
			{
				const std::string key = "Entity " + std::to_string(entity.GetId());
				scene.RenameEntity(entity, key);

				context.SetSelectedEntity(entity);
				context.MarkDirty();
			}
		}

		ImGui::Separator();

		for (Entity entity : scene.GetRegistry().GetAllEntities())
		{
			if (RenderEntity(scene, entity, context))
			{
				entityToDelete = entity;
			}
		}
	}

	ImGui::End();

	if (entityToDelete)
	{
		context.SetSelectedEntity(entityToDelete);
		DeleteSelectedEntity(scene, context);
	}

	RenderRenamePopup(scene, context);
#else
	(void)scene;
	(void)context;
#endif
}

bool SceneHierarchyPanel::DeleteSelectedEntity(Scene& scene, EditorContext& context)
{
	const Entity selectedEntity = context.GetSelectedEntity();

	if (!scene.GetRegistry().IsValid(selectedEntity))
	{
		return false;
	}

	scene.DestroyEntity(selectedEntity);
	context.ClearSelectedEntity();
	context.MarkDirty();

	return true;
}

std::string SceneHierarchyPanel::GetEntityLabel(const Scene& scene, Entity entity) const
{
	const std::string_view key = scene.GetEntityKey(entity);

	if (!key.empty())
	{
		return std::string(key);
	}

	if (const TagComponent* tag = entity.Get<TagComponent>();
		tag != nullptr && !tag->Tag.empty())
	{
		return tag->Tag;
	}

	return "Entity " + std::to_string(entity.GetId());
}

bool SceneHierarchyPanel::RenderEntity(Scene& scene, Entity entity, EditorContext& context)
{
#ifdef _DEBUG
	const std::string label = GetEntityLabel(scene, entity);
	const std::string uniqueLabel = label + "##" + std::to_string(entity.GetId());

	const bool isSelected = context.GetSelectedEntity() == entity;

	if (ImGui::Selectable(uniqueLabel.c_str(), isSelected))
	{
		context.SetSelectedEntity(entity);
	}

	if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
	{
		context.SetSelectedEntity(entity);
	}

	bool deleteRequested = false;

	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Rename"))
		{
			BeginRename(scene, entity);
		}

		if (ImGui::MenuItem("Delete"))
		{
			deleteRequested = true;
		}

		ImGui::EndPopup();
	}

	return deleteRequested;
#else
	(void)scene;
	(void)entity;
	(void)context;
	return false;
#endif
}

void SceneHierarchyPanel::BeginRename(Scene& scene, Entity entity)
{
	const std::string currentLabel = GetEntityLabel(scene, entity);

	std::snprintf(m_renameBuffer.data(), m_renameBuffer.size(), "%s", currentLabel.c_str());

	m_entityBeingRenamed = entity;
	m_isRenamePopupOpen = true;
}

void SceneHierarchyPanel::RenderRenamePopup(Scene& scene, EditorContext& context)
{
#ifdef _DEBUG
	if (m_isRenamePopupOpen)
	{
		ImGui::OpenPopup("Rename Entity");
		m_isRenamePopupOpen = false;
	}

	if (!ImGui::BeginPopupModal("Rename Entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		return;
	}

	ImGui::InputText("Name", m_renameBuffer.data(), m_renameBuffer.size());

	const bool hasName = m_renameBuffer[0] != '\0';

	if (ImGui::Button("Rename") && hasName)
	{
		if (scene.RenameEntity(m_entityBeingRenamed, m_renameBuffer.data()))
		{
			context.MarkDirty();
			ImGui::CloseCurrentPopup();
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Cancel"))
	{
		ImGui::CloseCurrentPopup();
	}

	ImGui::EndPopup();
#else
	(void)scene;
	(void)context;
#endif
}