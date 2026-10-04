#pragma once
#include "../../ECS/CameraComponent.h"
#include "../../Engine.h"
#include "../../World/Scene.h"
#include "../EditorContext.h"

class ViewportPanel
{
public:
	~ViewportPanel();

	void Render(Engine& engine, Scene& scene, EditorContext& context);

private:
	bool EnsureRenderTarget(Engine& engine, int width, int height);
	void ResetCamera();
	void RenderColliderBox(Registry& registry, SDL_Renderer* renderer);

private:
	SDL_Texture* m_renderTarget{ nullptr };
	CameraComponent m_camera;

	int m_renderTargetWidth{ 0 };
	int m_renderTargetHeight{ 0 };
};