#pragma once
#include "../ECS/Registry.h"
#include "../ECS/SpriteAnimationComponent.h"
#include "../ECS/SpriteComponent.h"
#include "../Managers/AnimationManager.h"
#include <string>

class AnimationStateSystem final
{
public:
	static void UpdateAnimationStates(Registry& registry, AnimationManager& animationManager);
		
	static void PlayAnimation(SpriteComponent& sprite, SpriteAnimationComponent& anim, AnimationManager& animationManager, AnimationState animationState);

	static void SetSpeed(SpriteAnimationComponent& anim, float speedMultiplier);
};