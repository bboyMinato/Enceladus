#include "AssetPanel.h"
#include "../../Utility/AssetFilePaths.h"

#include <algorithm>
#include <cctype>
#include <system_error>
#include <vector>

#ifdef _DEBUG
#include <imgui.h>
#endif

namespace
{
	bool CaseInsensitivePathLess(const std::filesystem::directory_entry& left, const std::filesystem::directory_entry& right)
	{
		std::error_code error;
		const bool leftIsDirectory = left.is_directory(error);

		error.clear();
		const bool rightIsDirectory = right.is_directory(error);

		if (leftIsDirectory != rightIsDirectory)
		{
			return leftIsDirectory;
		}

		std::string leftName = left.path().filename().string();
		std::string rightName = right.path().filename().string();

		std::transform(leftName.begin(), leftName.end(), leftName.begin(),
			[](unsigned char character) { return static_cast<char>(std::tolower(character)); });

		std::transform(rightName.begin(), rightName.end(), rightName.begin(),
			[](unsigned char character) { return static_cast<char>(std::tolower(character)); });

		return leftName < rightName;
	}
}

void AssetPanel::Render()
{
#ifdef _DEBUG
	if (m_assetsDirectory.empty())
	{
		Refresh();
	}

	if (!ImGui::Begin("Assets"))
	{
		ImGui::End();
		return;
	}

	if (!m_statusMessage.empty())
	{
		ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "%s", m_statusMessage.c_str());

		ImGui::End();
		return;
	}

	std::error_code error;
	const std::filesystem::path relativeDirectory = std::filesystem::relative(m_currentDirectory, m_assetsDirectory, error);

	const std::string breadcrumb = error || relativeDirectory.empty()
									? "Assets"
									: "Assets/" + relativeDirectory.generic_string();

	ImGui::TextUnformatted(breadcrumb.c_str());

	if (m_currentDirectory != m_assetsDirectory)
	{
		ImGui::SameLine();

		if (ImGui::Button("Up"))
		{
			m_currentDirectory = m_currentDirectory.parent_path();
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Refresh"))
	{
		Refresh();
	}

	ImGui::InputTextWithHint("##AssetFilter", "Filter assets...", m_filter.data(), m_filter.size());

	const std::string filter = m_filter.data();
	std::vector<std::filesystem::directory_entry> entries;

	for (std::filesystem::directory_iterator iterator(m_currentDirectory, std::filesystem::directory_options::skip_permission_denied, error);
		!error && iterator != std::filesystem::directory_iterator();
		iterator.increment(error))
	{
		const std::filesystem::directory_entry& entry = *iterator;

		if (!filter.empty())
		{
			std::string fileName = entry.path().filename().string();
			std::string loweredFilter = filter;

			std::transform(fileName.begin(), fileName.end(), fileName.begin(),
				[](unsigned char character) { return static_cast<char>(std::tolower(character)); });

			std::transform(loweredFilter.begin(), loweredFilter.end(), loweredFilter.begin(),
				[](unsigned char character) { return static_cast<char>(std::tolower(character)); });

			if (fileName.find(loweredFilter) == std::string::npos)
			{
				continue;
			}
		}

		entries.push_back(entry);
	}

	if (error)
	{
		m_statusMessage = "Unable to enumerate assets: " + error.message();
		ImGui::End();
		return;
	}

	std::sort(entries.begin(), entries.end(), CaseInsensitivePathLess);

	ImGui::Separator();

	for (const std::filesystem::directory_entry& entry : entries)
	{
		error.clear();
		const bool isDirectory = entry.is_directory(error);

		if (error)
		{
			continue;
		}

		const std::string fileName = entry.path().filename().string();
		const std::string label = (isDirectory ? "[Folder] " : "[Asset] ") + fileName;
		const bool isSelected = m_selectedAsset == entry.path();

		ImGui::PushID(entry.path().generic_string().c_str());

		if (ImGui::Selectable(label.c_str(), isSelected, ImGuiSelectableFlags_AllowDoubleClick))
		{
			m_selectedAsset = entry.path();

			if (isDirectory && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				m_currentDirectory = entry.path();
			}
		}

		if (!isDirectory && ImGui::BeginDragDropSource())
		{
			const std::filesystem::path relativeAsset = std::filesystem::relative(entry.path(), m_assetsDirectory, error);

			if (!error)
			{
				const std::string assetPath = relativeAsset.generic_string();

				ImGui::SetDragDropPayload("ENCELADUS_ASSET_PATH", assetPath.c_str(), assetPath.size() + 1);

				ImGui::TextUnformatted(assetPath.c_str());
			}

			ImGui::EndDragDropSource();
		}

		ImGui::PopID();
	}

	ImGui::End();
#endif
}

void AssetPanel::Refresh()
{
	m_assetsDirectory = AssetPaths::ResolveProjectAsset({});
	m_currentDirectory = m_assetsDirectory;
	m_selectedAsset.clear();
	m_statusMessage.clear();

	std::error_code error;

	if (!std::filesystem::is_directory(m_assetsDirectory, error))
	{
		m_statusMessage = "Assets directory was not found: " + m_assetsDirectory.string();
	}
}