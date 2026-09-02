#include "AssetFilePaths.h"

#include <SDL2/SDL.h>

const std::filesystem::path& AssetPaths::ExecutableDirectory()
{
	static const std::filesystem::path directory = []
	{
		char* basePath = SDL_GetBasePath();

		if (basePath == nullptr)
		{
			return std::filesystem::current_path();
		}

		std::filesystem::path result{ basePath };
		SDL_free(basePath);

		return result.lexically_normal();
	}();

	return directory;
}

std::filesystem::path AssetPaths::ResolveAsset(const std::filesystem::path& path)
{
	if (path.is_absolute())
	{
		return path.lexically_normal();
	}

	const auto firstComponent = path.begin();

	if (firstComponent != path.end() && *firstComponent == "Assets")
	{
		return (ExecutableDirectory() / path).lexically_normal();
	}

	return (ExecutableDirectory() / "Assets" / path).lexically_normal();
}

std::filesystem::path AssetPaths::ResolveAppFile(const std::filesystem::path& path)
{
	if (path.is_absolute())
	{
		return path.lexically_normal();
	}

	return (ExecutableDirectory() / path).lexically_normal();
}
