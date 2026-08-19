#pragma once

#include "../ECS/MovementComponent.h"
#include "../ECS/TransformComponent.h"
#include "../ECS/Registry.h"
#include <SDL2/SDL.h>
#include <cmath>

class MovementSystem final
{
public: 
	static void Update(Registry& registry, float deltaTime);

private:
	static void ApplyMovement(Entity& entity, Registry& registry, float deltaTime);
};