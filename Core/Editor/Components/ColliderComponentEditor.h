#pragma once
#include "../../ECS/Entity.h"

class ColliderComponentEditor
{
public:
	/// <summary>
	/// Renders the ColliderComponent of the given entity in the editor.
	/// </summary>
	/// <param name="entity">The entity whose ColliderComponent will be rendered.</param>
	static bool Render(Entity& entity);
};