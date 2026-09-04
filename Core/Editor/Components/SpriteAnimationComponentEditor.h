#pragma once
#include "../../ECS/Entity.h"

class Engine;

class SpriteAnimationComponentEditor
{
public:
	static bool Render(Engine& engine, Entity& entity);
};