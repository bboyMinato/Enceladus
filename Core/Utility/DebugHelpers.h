#pragma once

#ifdef _DEBUG

#include "../Engine.h"
#include "../ECS/Registry.h"

class DebugHelper
{
public:
	DebugHelper() = default;

	void RenderImGui(Engine& engine, Registry& registry);
	void RenderDebugCollider(Engine& engine, Registry& registry, Entity& camera);

private:
	void RenderEntityTab(Entity& selectedEntity);
	void RenderColliderTab(Entity& selectedEntity);
	void RenderSpriteTab(Entity& selectedEntity);
	void RenderAnimationTab(Entity& selectedEntity, Engine& engine);
	void RenderInteractionTab(Entity& selectedEntity);
	void RenderCameraTab(Entity& cameraEntity);

private:
	Uint8 m_selectedEntityId{ 1 };
	bool m_showColliderDebug{ true };
	bool m_showDebugWindow{ true };
};

#endif