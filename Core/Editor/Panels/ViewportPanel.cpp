#include "ViewportPanel.h"

#include <algorithm>
#include <cmath>

#ifdef _DEBUG
#include <imgui.h>
#endif
#include "../../Systems/SpriteAnimationSystem.h"
#include "../../ECS/ColliderComponent.h"
#include "../../ECS/TransformComponent.h"

ViewportPanel::~ViewportPanel()
{
	if (m_renderTarget != nullptr)
	{
		SDL_DestroyTexture(m_renderTarget);
		m_renderTarget = nullptr;
	}
}

void ViewportPanel::Render(Engine& engine, Scene& scene, EditorContext& context)
{
#ifdef _DEBUG
	(void)context;

	if (!ImGui::Begin("Viewport"))
	{
		ImGui::End();
		return;
	}

	if (ImGui::Button("Reset View"))
	{
		ResetCamera();
	}

	ImGui::SameLine();
	ImGui::Text("Camera: %d, %d", m_camera.m_viewport.x, m_camera.m_viewport.y);

	ImGui::Separator();

	const ImVec2 availableSize = ImGui::GetContentRegionAvail();
	const int viewportWidth = std::max(1, static_cast<int>(std::floor(availableSize.x)));
	const int viewportHeight = std::max(1, static_cast<int>(std::floor(availableSize.y)));

	if (!EnsureRenderTarget(engine, viewportWidth, viewportHeight))
	{
		ImGui::TextUnformatted("Unable to create the viewport render target.");
		ImGui::End();
		return;
	}

	m_camera.m_viewport.w = viewportWidth;
	m_camera.m_viewport.h = viewportHeight;

	// This is the top-left screen-space position where ImGui::Image will be drawn.
	const ImVec2 imagePosition = ImGui::GetCursorScreenPos();
	const ImVec2 mousePosition = ImGui::GetMousePos();

	// Convert screen-space mouse coordinates to world-space coordinates.
	const ImVec2 worldMousePosition
	{
		mousePosition.x - imagePosition.x + static_cast<float>(m_camera.m_viewport.x),
		mousePosition.y - imagePosition.y + static_cast<float>(m_camera.m_viewport.y)
	};

	SDL_Renderer* renderer = engine.GetRenderSystem().GetRenderer();
	SDL_Texture* previousRenderTarget = SDL_GetRenderTarget(renderer);

	SDL_SetRenderTarget(renderer, m_renderTarget);
	SDL_SetRenderDrawColor(renderer, 30, 30, 34, 255);
	SDL_RenderClear(renderer);

	const TileMap& tileMap = scene.GetTileMap();
	const bool hasTileMap =
		tileMap.m_mapWidth > 0 &&
		tileMap.m_mapHeight > 0 &&
		tileMap.m_tileWidth > 0 &&
		tileMap.m_tileHeight > 0 &&
		!tileMap.tileSets.empty() &&
		!tileMap.tileLayers.empty();

	AnimationSystem::Update(scene.GetRegistry(), engine.GetAnimationManager(), 0.0f);

	if (hasTileMap)
	{
		engine.GetMapSystem().Render(tileMap, engine.GetRenderSystem(), m_camera, scene.GetRegistry());
	}
	else
	{
		engine.GetRenderSystem().RenderEntites(scene.GetRegistry(), m_camera);
	}

	RenderColliderBox(scene.GetRegistry(), renderer);

	SDL_SetRenderTarget(renderer, previousRenderTarget);

	ImGui::Image(reinterpret_cast<ImTextureID>(m_renderTarget), ImVec2(static_cast<float>(viewportWidth), static_cast<float>(viewportHeight)));

	const bool isViewportHovered = ImGui::IsItemHovered();

	if (isViewportHovered)
	{
		ImGui::SetTooltip("World position: %.0f, %.0f", worldMousePosition.x, worldMousePosition.y);
	}

	if (isViewportHovered && ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
	{
		const ImVec2 mouseDelta = ImGui::GetIO().MouseDelta;

		m_camera.m_viewport.x -= static_cast<int>(mouseDelta.x);
		m_camera.m_viewport.y -= static_cast<int>(mouseDelta.y);

		if (hasTileMap)
		{
			const int maxX = std::max(0, tileMap.GetPixelWidth() - m_camera.m_viewport.w);
			const int maxY = std::max(0, tileMap.GetPixelHeight() - m_camera.m_viewport.h);

			m_camera.m_viewport.x = std::clamp(m_camera.m_viewport.x, 0, maxX);
			m_camera.m_viewport.y = std::clamp(m_camera.m_viewport.y, 0, maxY);
		}
	}

	ImGui::End();
#else
	(void)engine;
	(void)scene;
	(void)context;
#endif
}

bool ViewportPanel::EnsureRenderTarget(Engine& engine, int width, int height)
{
	if (m_renderTarget != nullptr &&
		m_renderTargetWidth == width &&
		m_renderTargetHeight == height)
	{
		return true;
	}

	if (m_renderTarget != nullptr)
	{
		SDL_DestroyTexture(m_renderTarget);
		m_renderTarget = nullptr;
	}

	SDL_Renderer* renderer = engine.GetRenderSystem().GetRenderer();

	if (renderer == nullptr)
	{
		return false;
	}

	m_renderTarget = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);

	if (m_renderTarget == nullptr)
	{
		m_renderTargetWidth = 0;
		m_renderTargetHeight = 0;

		return false;
	}

	SDL_SetTextureBlendMode(m_renderTarget, SDL_BLENDMODE_BLEND);
	SDL_SetTextureScaleMode(m_renderTarget, SDL_ScaleModeNearest);

	m_renderTargetWidth = width;
	m_renderTargetHeight = height;

	return true;
}

void ViewportPanel::ResetCamera()
{
	m_camera.m_viewport.x = 0;
	m_camera.m_viewport.y = 0;
}

void ViewportPanel::RenderColliderBox(Registry& registry, SDL_Renderer* renderer)
{
	registry.ForEach<ColliderComponent>([this, renderer](Entity entity, ColliderComponent& collider)
		{
			const TransformComponent* transform = entity.Get<TransformComponent>();

			if (transform == nullptr)
			{
				return;
			}

			SDL_Rect colliderRect
			{
				collider.m_offsetX + static_cast<int>(transform->x) - m_camera.m_viewport.x,
				collider.m_offsetY + static_cast<int>(transform->y) - m_camera.m_viewport.y,
				collider.m_width,
				collider.m_height
			};

			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderDrawRect(renderer, &colliderRect);
		}
	);
}