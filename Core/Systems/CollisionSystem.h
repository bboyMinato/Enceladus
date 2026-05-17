#pragma once
#include "../ECS/Registry.h"
#include <SDL2/SDL.h>

class CollisionSystem
{
public:
	static void Update(Registry& registry);

private:
	static void ResolveCollision(Entity* entityA, Entity* entityB, SDL_Rect& rectA, SDL_Rect& rectB);
	static SDL_Rect GetRect(Entity* entity);
};