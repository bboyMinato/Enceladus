#pragma once
#include "../../ECS/Entity.h"
#include "../../World/Scene.h"

class SpriteComponentEditor
{
public:
	/// <summary>
	/// Renders the SpriteComponent of the given entity in the editor.
	/// </summary>
	/// <param name="entity">The entity whose SpriteComponent will be rendered.</param>
	static bool Render(Entity& entity, const Scene& scene);
};