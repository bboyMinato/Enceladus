#pragma once
#include "../ECS/Registry.h"

class AnimationStateSystem final
{
public:
	static void UpdateAnimationStates(Registry& registry);
};