#pragma once
#include <array>
#include <filesystem>
#include <string>

class AssetPanel
{
public:
	void Render();
	
private:
	void Refresh();

	std::filesystem::path m_assetsDirectory;
	std::filesystem::path m_currentDirectory;
	std::filesystem::path m_selectedAsset;

	std::array<char, 128> m_filter{};
	std::string m_statusMessage;
};