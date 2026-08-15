#pragma once
#include "../ECS/Registry.h"
#include "../ECS/SpriteComponent.h"
#include "../ECS/SpriteAnimationComponent.h"
#include "../Managers/AnimationManager.h"
#include <string>

class AnimationStateSystem final
{
public:
	static void UpdateAnimationStates(Registry& registry, AnimationManager& animationManager);

	static void PlayAnimation(SpriteComponent& sprite, SpriteAnimationComponent& anim, AnimationManager& animationManager, std::string_view animationName);

	static void SetSpeed(SpriteAnimationComponent& anim, float speedMultiplier);
};