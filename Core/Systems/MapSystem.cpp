#include "MapSystem.h"

#include <algorithm>

namespace
{
	constexpr std::uint32_t HorizontalFlipFlag = 0x80000000u;
	constexpr std::uint32_t VerticalFlipFlag = 0x40000000u;
	constexpr std::uint32_t DiagonalFlipFlag = 0x20000000u;

	std::uint32_t StripTileFlags(std::uint32_t gid)
	{
		return gid & ~(HorizontalFlipFlag | VerticalFlipFlag | DiagonalFlipFlag);
	}
}

void MapSystem::Render(const TileMap& tileMap, const RenderSystem& renderSystem, const CameraComponent& camera, Registry& registry)
{
	if (!IsLoaded(tileMap))
	{
		return;
	}

	bool hasRenderedEntities = false;

	for (const TileLayer& layer : tileMap.tileLayers)
	{
		if (!layer.isVisible)
		{
			continue;
		}

		RenderLayer(tileMap, layer, renderSystem, camera.m_viewport);

		if (layer.name == "walls" && !hasRenderedEntities)
		{
			renderSystem.RenderEntites(registry, camera);
			hasRenderedEntities = true;
		}
	}

	if (!hasRenderedEntities)
	{
		renderSystem.RenderEntites(registry, camera);
	}
}

#pragma region Private functions

bool MapSystem::IsLoaded(const TileMap& tileMap)
{
	return tileMap.m_mapWidth > 0 &&
		tileMap.m_mapHeight > 0 &&
		tileMap.m_tileWidth > 0 &&
		tileMap.m_tileHeight > 0 &&
		!tileMap.tileSets.empty() &&
		!tileMap.tileLayers.empty();
}

const TileSet* MapSystem::FindTileSetForGid(const TileMap& tileMap, std::uint32_t gid)
{
	gid = StripTileFlags(gid);

	if (gid == 0)
	{
		return nullptr;
	}

	for (auto tileSetIt = tileMap.tileSets.rbegin(); tileSetIt != tileMap.tileSets.rend(); ++tileSetIt)
	{
		if (gid < static_cast<std::uint32_t>(tileSetIt->firstGid))
		{
			continue;
		}

		const std::uint32_t localTileId = gid - static_cast<std::uint32_t>(tileSetIt->firstGid);

		if (localTileId < static_cast<std::uint32_t>(tileSetIt->tileCount))
		{
			return &(*tileSetIt);
		}
	}

	return nullptr;
}

void MapSystem::RenderLayer(const TileMap& tileMap, const TileLayer& layer, const RenderSystem& renderSystem, const SDL_Rect& camera)
{
	const int startColumn = std::max(0, camera.x / tileMap.m_tileWidth);
	const int endColumn = std::min(layer.width, ((camera.x + camera.w) / tileMap.m_tileWidth) + 1);

	const int startRow = std::max(0, camera.y / tileMap.m_tileHeight);
	const int endRow = std::min(layer.height, ((camera.y + camera.h) / tileMap.m_tileHeight) + 1);

	for (int row = startRow; row < endRow; ++row)
	{
		for (int column = startColumn; column < endColumn; ++column)
		{
			const size_t tileIndex =
				static_cast<size_t>(row) * static_cast<size_t>(layer.width) +
				static_cast<size_t>(column);

			const std::uint32_t gid = StripTileFlags(layer.tiles[tileIndex]);
			if (gid == 0)
			{
				continue;
			}

			const TileSet* tileSet = FindTileSetForGid(tileMap, gid);
			if (tileSet == nullptr)
			{
				continue;
			}

			const int localTileId = static_cast<int>(gid) - tileSet->firstGid;

			const SDL_Rect sourceRect
			{
				.x = (localTileId % tileSet->columns) * tileSet->tileWidth,
				.y = (localTileId / tileSet->columns) * tileSet->tileHeight,
				.w = tileSet->tileWidth,
				.h = tileSet->tileHeight
			};

			const SDL_Rect destinationRect
			{
				.x = (column * tileMap.m_tileWidth) - camera.x,
				.y = (row * tileMap.m_tileHeight) - camera.y,
				.w = tileMap.m_tileWidth,
				.h = tileMap.m_tileHeight
			};

			renderSystem.RenderTexture(tileSet->textureName, &sourceRect, &destinationRect);
		}
	}
}

#pragma endregion