#pragma once

#include <filesystem>

class AssetPaths final
{
public:
	static const std::filesystem::path& ExecutableDirectory();
	static std::filesystem::path ResolveAsset(const std::filesystem::path& path);
	static std::filesystem::path ResolveAppFile(const std::filesystem::path& path);

private:
	AssetPaths() = delete;
};