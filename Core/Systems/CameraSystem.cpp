#include "CameraSystem.h"
#include <algorithm>

void CameraSystem::UpdateFollow(CameraComponent& camera, const TransformComponent& target, const SpriteComponent& sprite, RenderSystem& renderSystem, const TileMap& tileMap)
{
	if (!camera.m_shouldFollow)
	{
		return;
	}
	
	int windowWidth = 0;
	int windowHeight = 0;
	renderSystem.GetOutputSize(windowWidth, windowHeight);

	camera.m_viewport.w = windowWidth;
	camera.m_viewport.h = windowHeight;
	
	const int targetCenterX = static_cast<int>(target.x) + (sprite.m_width / 2);
	const int targetCenterY = static_cast<int>(target.y) + (sprite.m_height / 2);

	camera.m_viewport.x = targetCenterX - (camera.m_viewport.w / 2) + camera.m_followOffset.x;
	camera.m_viewport.y = targetCenterY - (camera.m_viewport.h / 2) + camera.m_followOffset.y;

	camera.m_viewport.x = std::clamp(camera.m_viewport.x, 0, std::max(0, tileMap.GetPixelWidth() - camera.m_viewport.w));
	camera.m_viewport.y = std::clamp(camera.m_viewport.y, 0, std::max(0, tileMap.GetPixelHeight() - camera.m_viewport.h));
}

void CameraSystem::Update(Registry& registry, Entity& cameraEntity, RenderSystem& renderSystem, const TileMap& tileMap)
{
    CameraComponent* camera = cameraEntity.Get<CameraComponent>();
    TransformComponent* cameraTransform = cameraEntity.Get<TransformComponent>();

    if (camera == nullptr || cameraTransform == nullptr || !camera->m_isActive)
    {
        return;
    }

    int windowWidth = 0;
    int windowHeight = 0;
    renderSystem.GetOutputSize(windowWidth, windowHeight);

    camera->m_viewport.w = windowWidth;
    camera->m_viewport.h = windowHeight;

    switch (camera->m_mode)
    {
    case CameraMode::Follow:
    {
        if (!registry.IsValid(camera->m_target))
        {
            return;
        }

        const TransformComponent* targetTransform = camera->m_target.Get<TransformComponent>();

        if (targetTransform == nullptr)
        {
            return;
        }

        const SpriteComponent* targetSprite = camera->m_target.Get<SpriteComponent>();

        float targetCenterX = targetTransform->x;
        float targetCenterY = targetTransform->y;

        if (targetSprite != nullptr)
        {
            targetCenterX += targetSprite->m_width / 2.0f;
            targetCenterY += targetSprite->m_height / 2.0f;
        }

        cameraTransform->x = targetCenterX - (camera->m_viewport.w / 2.0f) + camera->m_followOffset.x;
        cameraTransform->y = targetCenterY - (camera->m_viewport.h / 2.0f) + camera->m_followOffset.y;

        break;
    }

    case CameraMode::Free:
        // A free-camera controller changes cameraTransform->x/y.
        // The camera system only converts that transform into a viewport.
        break;

    case CameraMode::Fixed:
        // Keep cameraTransform unchanged.
        break;
    }

    cameraTransform->x = std::clamp(
        cameraTransform->x,
        0.0f,
        static_cast<float>(std::max(0, tileMap.GetPixelWidth() - camera->m_viewport.w)));

    cameraTransform->y = std::clamp(
        cameraTransform->y,
        0.0f,
        static_cast<float>(std::max(0, tileMap.GetPixelHeight() - camera->m_viewport.h)));

    camera->m_viewport.x = static_cast<int>(cameraTransform->x);
    camera->m_viewport.y = static_cast<int>(cameraTransform->y);
}