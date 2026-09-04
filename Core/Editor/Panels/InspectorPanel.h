#pragma once
#include "../../ECS/Entity.h"
#include "../../Engine.h"
#include "../../World/Scene.h"
#include "../EditorContext.h"
#include "../Components/TagComponentEditor.h"

class InspectorPanel
{
public:
	void Render(Engine& engine, Scene& scene, EditorContext& context);

private:
	bool RenderEntity(Engine& engine, Entity& entity, const Scene& scene);
	bool RenderAddComponent(Engine& engine, Scene& scene, Entity& entity);

private:
	TagComponentEditor m_tagComponentEditor;
};