#include "AssetFilePaths.h"

#include <SDL2/SDL.h>

namespace
{
	std::filesystem::path FindProjectDirectory()
	{
		std::filesystem::path directory = AssetPaths::ExecutableDirectory();

		while (!directory.empty())
		{
			std::error_code error;
			std::filesystem::directory_iterator entries(directory, std::filesystem::directory_options::skip_permission_denied, error);

			for (const std::filesystem::directory_entry& entry : entries)
			{
				if (entry.is_regular_file(error) && entry.path().extension() == ".vcxproj")
				{
					return directory;
				}
			}

			const std::filesystem::path parent = directory.parent_path();

			if (parent == directory)
			{
				break;
			}

			directory = parent;
		}

		return AssetPaths::ExecutableDirectory();
	}

	const std::filesystem::path& ProjectDirectory()
	{
		static const std::filesystem::path projectDirectory =
			FindProjectDirectory();

		return projectDirectory;
	}
}

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

std::filesystem::path AssetPaths::ResolveProjectAsset(const std::filesystem::path& path)
{
	if (path.is_absolute())
	{
		return path.lexically_normal();
	}

	const auto firstComponent = path.begin();

	if (firstComponent != path.end() && *firstComponent == "Assets")
	{
		return (ProjectDirectory() / path).lexically_normal();
	}

	return (ProjectDirectory() / "Assets" / path).lexically_normal();
}

std::filesystem::path AssetPaths::ResolveAppFile(const std::filesystem::path& path)
{
	if (path.is_absolute())
	{
		return path.lexically_normal();
	}

	return (ExecutableDirectory() / path).lexically_normal();
}
