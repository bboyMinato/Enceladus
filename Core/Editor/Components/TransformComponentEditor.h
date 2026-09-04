#pragma once
#include "../../ECS/Entity.h"

class TransformComponentEditor
{
public:
	/// <summary>
	/// Renders the TransformComponent of the given entity in the editor.
	/// </summary>
	/// <param name="entity">The entity whose TransformComponent will be rendered.</param>
	static bool Render(Entity& entity);
};