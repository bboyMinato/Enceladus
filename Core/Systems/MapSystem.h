#pragma once

#include "../World/TileMap.h"
#include "../Systems/RenderSystem.h"
#include "../ECS/CameraComponent.h"
#include "../ECS/Registry.h"

class MapSystem final
{
public:
	static void Render(const TileMap& tileMap, const RenderSystem& renderSystem, const CameraComponent& camera, Registry& registry);

private:
	static bool IsLoaded(const TileMap& tileMap);

	static const TileSet* FindTileSetForGid(const TileMap& tileMap, std::uint32_t gid);

	static void RenderLayer(const TileMap& tileMap, const TileLayer& layer, const RenderSystem& renderSystem, const SDL_Rect& camera);
};