#pragma once
#include "../Engine.h"
#include "../World/Scene.h"
#include "EditorContext.h"
#include "Panels/AssetPanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ViewportPanel.h"
#include <array>
#include <filesystem>
#include <optional>
#include <string>

class Editor
{
public:
	void Initialize(Engine& engine, const std::filesystem::path& scenePath);
	void Render(Engine& engine, Scene& scene);

	bool RequestClose();

	std::optional<std::filesystem::path> ConsumeOpenSceneRequest();
	void SetErrorStatus(std::string message);

	[[nodiscard]]
	const std::filesystem::path& GetScenePath() const
	{
		return m_context.GetScenePath();
	}

private:
	void SaveCurrentScene(Scene& scene);
	bool SaveSceneToPath(Scene& scene, const std::filesystem::path& scenePath);

	void RenderSaveAsDialog(Scene& scene);
	void RenderUnsavedChangesDialog(Engine& engine, Scene& scene);

	void RenderOpenSceneDialog();
	void RequestOpenScene(std::filesystem::path scenePath);

	void CompletePendingAction(Engine& engine);
	void ClearPendingAction();
private:
	enum class PendingAction
	{
		None,
		CloseEditor,
		OpenScene
	};

	EditorContext m_context;

	SceneHierarchyPanel m_sceneHierarchy;
	InspectorPanel m_inspector;
	AssetPanel m_assetPanel;
	ViewportPanel m_viewportPanel;

	std::array<char, 260> m_saveAsPathBuffer{};
	std::string m_saveStatusMessage;

	std::array<char, 260> m_openScenePathBuffer{};
	std::optional<std::filesystem::path> m_openSceneRequest;

	PendingAction m_pendingAction{ PendingAction::None };
	std::optional<std::filesystem::path> m_pendingScenePath;

	bool m_lastSaveFailed{ false };
	bool m_openSaveAsPopup{ false };
	bool m_openUnsavedChangesPopup{ false };
	bool m_openOpenScenePopup{ false };

	bool m_showSceneHierarchy{ true };
	bool m_showInspector{ true };
	bool m_showAssets{ true };
	bool m_showViewport{ true };
};