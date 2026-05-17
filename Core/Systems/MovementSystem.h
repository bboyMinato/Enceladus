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
	static void ApplyMovement(Entity& entity, Registry& registry, float deltaTime);

private:
	static SDL_Rect GetRect(Entity& entity);
	static void ResolveCollisionOnX(Entity& entity, Registry& registry);
	static void ResolveCollisionOnY(Entity& entity, Registry& registry);
};