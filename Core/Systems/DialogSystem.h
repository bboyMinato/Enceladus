#pragma once
#include "../ECS/Registry.h"

class DialogSystem
{
public:
	DialogSystem() = default;

	static void Update(Registry& registry, float deltaTime);
};
