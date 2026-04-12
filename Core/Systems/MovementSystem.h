#pragma once

#include "../ECS/MovementComponent.h"
#include "../ECS/TransformComponent.h"
#include "../ECS/Registry.h"
#include <cmath>

class MovementSystem final
{
public: 
	static void Update(Registry& registry, float deltaTime);
	static void ApplyMovement(TransformComponent& transform, const MovementComponent& movement, float deltaTime);
};