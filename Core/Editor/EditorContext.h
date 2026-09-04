#pragma once
#include "../ECS/Entity.h"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <utility>

class EditorContext
{
public:
	void ResetForScene(const std::filesystem::path& scenePath)
	{
		m_scenePath = scenePath;
		ClearSelectedEntity();
		ClearSelectedTileLayer();
		ClearSelectedTile();
		ClearDirty();
	}

	/// <summary>
	/// Sets the currently selected entity in the editor context.
	/// </summary>
	/// <param name="entity">The entity to select.</param>
	void SetSelectedEntity(Entity entity) { m_selectedEntity = entity; }

	/// <summary>
	/// Gets the currently selected entity in the editor context.
	/// </summary>
	/// <returns>The currently selected entity.</returns>
	Entity GetSelectedEntity() const { return m_selectedEntity; }
	
	/// <summary>
	/// Clears the currently selected entity in the editor context.
	/// </summary>
	void ClearSelectedEntity() { m_selectedEntity = {}; }

	[[nodiscard]]
	bool HasValidSelectedEntity(const Registry& registry) const
	{
		return registry.IsValid(m_selectedEntity);
	}

	void ValidateSelection(const Registry& registry)
	{
		if (!HasValidSelectedEntity(registry))
		{
			ClearSelectedEntity();
		}
	}

	void SetSelectedTileLayer(std::size_t layerIndex)
	{
		m_selectedTileLayer = layerIndex;
	}

	[[nodiscard]]
	const std::optional<std::size_t>& GetSelectedTileLayer() const
	{
		return m_selectedTileLayer;
	}

	void ClearSelectedTileLayer() { m_selectedTileLayer.reset(); }

	void SetSelectedTile(std::uint32_t tileId)
	{
		m_selectedTile = tileId;
	}

	[[nodiscard]]
	const std::optional<std::uint32_t>& GetSelectedTile() const
	{
		return m_selectedTile;
	}

	void ClearSelectedTile() { m_selectedTile.reset(); }

	void SetScenePath(std::filesystem::path scenePath)
	{
		m_scenePath = std::move(scenePath);
	}

	[[nodiscard]]
	const std::filesystem::path& GetScenePath() const
	{
		return m_scenePath;
	}

	void MarkDirty() { m_isDirty = true; }
	void ClearDirty() { m_isDirty = false; }

	[[nodiscard]]
	bool IsDirty() const { return m_isDirty; }

private:
	/// <summary>
	/// The currently selected entity in the editor context.
	/// </summary>
	Entity m_selectedEntity;
	std::optional<std::size_t> m_selectedTileLayer;
	std::optional<std::uint32_t> m_selectedTile;

	std::filesystem::path m_scenePath;
	bool m_isDirty{ false };
};