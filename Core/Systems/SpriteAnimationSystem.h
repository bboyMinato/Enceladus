#pragma once

#include "../ECS/SpriteAnimationComponent.h"
#include "../ECS/Registry.h"
#include "../ECS/SpriteComponent.h"
#include "../Managers/AnimationManager.h"

class AnimationSystem final
{
public:
	static void Update(Registry& registry, AnimationManager& animationManager, float deltaTime);

private:
	static void UpdateSprite(SpriteComponent& sprite, SpriteAnimationComponent& animation, AnimationManager& animationManager, float deltaTime);
};