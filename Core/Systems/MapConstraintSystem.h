#pragma once

#include "../ECS/SpriteComponent.h"
#include "../ECS/TransformComponent.h"
#include "../World/TileMap.h"

class MapConstraintSystem final
{
public:
	static void ClampToTileMap(TransformComponent& transform, const SpriteComponent& sprite, const TileMap& tileMap);
};