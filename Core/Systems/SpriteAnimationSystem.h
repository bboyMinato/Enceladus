#pragma once

#include "../ECS/SpriteAnimationComponent.h"
#include "../ECS/Registry.h"
#include "../ECS/SpriteComponent.h"

class SpriteAnimationSystem final
{
public:
	static void Update(Registry& registry, float deltaTime);

private:
	static void UpdateSprite(SpriteComponent& sprite, SpriteAnimationComponent& animation, float deltaTime);
};