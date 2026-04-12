#include "MapConstraintSystem.h"
#include <algorithm>

void MapConstraintSystem::ClampToTileMap(TransformComponent& transform, const SpriteComponent& sprite, const TileMap& tileMap)
{
	transform.x = std::clamp(transform.x, 0.0f, static_cast<float>(std::max(0, tileMap.GetPixelWidth() - sprite.m_width)));
	transform.y = std::clamp(transform.y, 0.0f, static_cast<float>(std::max(0, tileMap.GetPixelHeight() - sprite.m_height)));
}