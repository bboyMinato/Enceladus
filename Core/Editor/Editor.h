#pragma once
#include "../Engine.h"
#include "../World/Scene.h"
#include "EditorContext.h"
#include "Panels/AssetPanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include <array>
#include <filesystem>
#include <string>

class Editor
{
public:
	void Initialize(Engine& engine, const std::filesystem::path& scenePath);
	void Render(Engine& engine, Scene& scene);

	bool RequestClose();

private:
	void SaveCurrentScene(Scene& scene);
	bool SaveSceneToPath(Scene& scene, const std::filesystem::path& scenePath);

	void RenderSaveAsDialog(Scene& scene);
	void RenderUnsavedChangesDialog(Engine& engine, Scene& scene);

private:
	EditorContext m_context;

	SceneHierarchyPanel m_sceneHierarchy;
	InspectorPanel m_inspector;
	AssetPanel m_assetPanel;

	std::array<char, 260> m_saveAsPathBuffer{};
	std::string m_saveStatusMessage;

	bool m_lastSaveFailed{ false };
	bool m_openSaveAsPopup{ false };
	bool m_openUnsavedChangesPopup{ false };

	bool m_showSceneHierarchy{ true };
	bool m_showInspector{ true };
	bool m_showAssets{ true };
};