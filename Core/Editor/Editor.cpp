#include "Editor.h"
#include "../World/SceneLoader.h"
#include <cstdio>

#ifdef _DEBUG
#include <imgui.h>
#endif

void Editor::Initialize(Engine& engine, const std::filesystem::path& scenePath)
{
	(void)engine;

	std::snprintf(m_saveAsPathBuffer.data(), m_saveAsPathBuffer.size(),	"%s", scenePath.generic_string().c_str());

	m_context.ResetForScene(scenePath);
	m_saveStatusMessage.clear();
	m_lastSaveFailed = false;
	m_openUnsavedChangesPopup = false;
	m_openSaveAsPopup = false;
}

void Editor::Render(Engine& engine, Scene& scene)
{
#ifdef _DEBUG
	/*ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());*/

	m_context.ValidateSelection(scene.GetRegistry());

	bool saveRequested = false;
	bool saveAsRequested = false;
	bool deleteRequested = false;

	const bool canSave = !m_context.GetScenePath().empty();
	const bool canDelete =	m_context.HasValidSelectedEntity(scene.GetRegistry());

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("New Scene", nullptr, false, false);
			ImGui::MenuItem("Open Scene...", nullptr, false, false);
			
			if (ImGui::MenuItem("Save", "Ctrl+S", false, canSave))
			{
				saveRequested = true;
			}
			
			if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
			{
				saveAsRequested = true;
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Close Editor", "Escape"))
			{
				if (RequestClose())
				{
					engine.PopState();
				}
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::MenuItem("Undo", "Ctrl+Z", false, false);
			ImGui::MenuItem("Redo", "Ctrl+Y", false, false);
			ImGui::MenuItem("Duplicate", "Ctrl+D", false, false);

			if (ImGui::MenuItem("Delete", "Delete", false, canDelete))
			{
				deleteRequested = true;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			ImGui::MenuItem("Scene Hierarchy", nullptr, &m_showSceneHierarchy);
			ImGui::MenuItem("Inspector", nullptr, &m_showInspector);
			ImGui::MenuItem("Assets", nullptr, &m_showAssets);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	const bool saveShortcutPressed = canSave && !ImGui::GetIO().WantTextInput && ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift && ImGui::IsKeyPressed(ImGuiKey_S);

	if (saveRequested || saveShortcutPressed)
	{
		SaveCurrentScene(scene);
	}

	const bool saveAsShortcutPressed = !ImGui::GetIO().WantTextInput && ImGui::GetIO().KeyCtrl && ImGui::GetIO().KeyShift && ImGui::IsKeyPressed(ImGuiKey_S);

	if (saveAsRequested || saveAsShortcutPressed)
	{
		m_openSaveAsPopup = true;
	}

	const bool deleteKeyPressed = canDelete && !ImGui::GetIO().WantTextInput && ImGui::IsKeyPressed(ImGuiKey_Delete);

	if (deleteRequested || deleteKeyPressed)
	{
		m_sceneHierarchy.DeleteSelectedEntity(scene, m_context);
	}

	if (m_showSceneHierarchy)
	{
		m_sceneHierarchy.Render(scene, m_context);
	}

	if (m_showInspector)
	{
		m_inspector.Render(engine, scene, m_context);
	}

	if (m_showAssets)
	{
		m_assetPanel.Render();
	}

	if (!m_saveStatusMessage.empty())
	{
		if (ImGui::Begin("Editor Status"))
		{
			const ImVec4 color = m_lastSaveFailed ? ImVec4(1.0f, 0.35f, 0.35f, 1.0f) : ImVec4(0.35f, 1.0f, 0.45f, 1.0f);

			ImGui::TextColored(color, "%s", m_saveStatusMessage.c_str());
		}

		ImGui::End();
	}

	RenderSaveAsDialog(scene);
	RenderUnsavedChangesDialog(engine, scene);
#else
	(void)engine;
	(void)scene;
#endif
}

bool Editor::RequestClose()
{
	if (!m_context.IsDirty())
	{
		return true;
	}

	m_openUnsavedChangesPopup = true;

	return false;
}

void Editor::SaveCurrentScene(Scene& scene)
{
	SaveSceneToPath(scene, m_context.GetScenePath());
}

bool Editor::SaveSceneToPath(Scene& scene, const std::filesystem::path& scenePath)
{
	const auto result = SceneLoader::SaveScene(scenePath, scene);

	if (!result)
	{
		m_saveStatusMessage = result.error();
		m_lastSaveFailed = true;
		return false;
	}

	m_context.SetScenePath(scenePath);
	m_context.ClearDirty();

	m_saveStatusMessage = "Saved " + scenePath.generic_string();

	m_lastSaveFailed = false;

	return true;
}

void Editor::RenderSaveAsDialog(Scene& scene)
{
#ifdef _DEBUG
	if (m_openSaveAsPopup)
	{
		ImGui::OpenPopup("Save Scene As");
		m_openSaveAsPopup = false;
	}

	if (!ImGui::BeginPopupModal("Save Scene As", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		return;
	}

	ImGui::TextUnformatted("Path relative to the Assets directory:");
	ImGui::InputText("Scene Path", m_saveAsPathBuffer.data(), m_saveAsPathBuffer.size());

	if (ImGui::Button("Save"))
	{
		std::filesystem::path scenePath(m_saveAsPathBuffer.data());

		if (scenePath.empty())
		{
			m_saveStatusMessage = "Scene path cannot be empty.";
			m_lastSaveFailed = true;
		}
		else
		{
			if (scenePath.extension() != ".json")
			{
				scenePath.replace_extension(".json");
			}

			if (SaveSceneToPath(scene, scenePath))
			{
				ImGui::CloseCurrentPopup();
			}
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
#endif
}

void Editor::RenderUnsavedChangesDialog(Engine& engine, Scene& scene)
{
#ifdef _DEBUG
	if (m_openUnsavedChangesPopup)
	{
		ImGui::OpenPopup("Unsaved Changes");
		m_openUnsavedChangesPopup = false;
	}

	if (!ImGui::BeginPopupModal("Unsaved Changes", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		return;
	}

	ImGui::TextUnformatted("This scene has unsaved changes.\n" "Save before closing the editor?");

	if (ImGui::Button("Save"))
	{
		SaveCurrentScene(scene);

		if (!m_context.IsDirty())
		{
			ImGui::CloseCurrentPopup();
			engine.PopState();
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Discard"))
	{
		ImGui::CloseCurrentPopup();
		engine.PopState();
	}

	ImGui::SameLine();

	if (ImGui::Button("Cancel"))
	{
		ImGui::CloseCurrentPopup();
	}

	ImGui::EndPopup();
#else
	(void)engine;
	(void)scene;
#endif
}
