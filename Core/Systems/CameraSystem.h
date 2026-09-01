#pragma once

#include "../ECS/CameraComponent.h"
#include "../ECS/SpriteComponent.h"
#include "../ECS/TransformComponent.h"
#include "../Systems/RenderSystem.h"
#include "../World/TileMap.h"

class CameraSystem final
{
public:
	
	static void Update(Registry& registry, Entity& cameraEntity, RenderSystem& renderSystem, const TileMap& tileMap);	
};