#include "CameraSystem.h"
#include <algorithm>

void CameraSystem::UpdateFollow(CameraComponent& camera, const TransformComponent& target, const SpriteComponent& sprite, RenderSystem& renderSystem, const TileMap& tileMap)
{
	int windowWidth = 0;
	int windowHeight = 0;
	renderSystem.GetOutputSize(windowWidth, windowHeight);

	camera.m_viewport.w = windowWidth;
	camera.m_viewport.h = windowHeight;

	if (!camera.m_shouldFollow)
	{
		return;
	}

	const int targetCenterX = static_cast<int>(target.x) + (sprite.m_width / 2);
	const int targetCenterY = static_cast<int>(target.y) + (sprite.m_height / 2);

	camera.m_viewport.x = targetCenterX - (camera.m_viewport.w / 2) + camera.m_offsetX;
	camera.m_viewport.y = targetCenterY - (camera.m_viewport.h / 2) + camera.m_offsetY;

	camera.m_viewport.x = std::clamp(camera.m_viewport.x, 0, std::max(0, tileMap.GetPixelWidth() - camera.m_viewport.w));
	camera.m_viewport.y = std::clamp(camera.m_viewport.y, 0, std::max(0, tileMap.GetPixelHeight() - camera.m_viewport.h));
}