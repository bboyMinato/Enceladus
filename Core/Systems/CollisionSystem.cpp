#include "CollisionSystem.h"
#include "../ECS/ColliderComponent.h"
#include "../ECS/TransformComponent.h"
#include <algorithm>

void CollisionSystem::Update(Registry& registry)
{
	std::vector<Entity> entities = registry.GetEntitiesWithComponents<ColliderComponent, TransformComponent>();

	for (size_t i = 0; i < entities.size(); ++i)
	{
		for (size_t j = i + 1; j < entities.size(); ++j)
		{
			Entity entityA = entities[i];
			Entity entityB = entities[j];

			const ColliderComponent* colliderA = entityA.Get<ColliderComponent>();
			const ColliderComponent* colliderB = entityB.Get<ColliderComponent>();

			if (colliderA == nullptr || colliderB == nullptr)
			{
				continue;
			}

			SDL_Rect rectA = GetRect(&entityA);
			SDL_Rect rectB = GetRect(&entityB);

			if (SDL_HasIntersection(&rectA, &rectB))
			{
				ResolveCollision(&entityA, &entityB, rectA, rectB);
			}
		}
	}
}

SDL_Rect CollisionSystem::GetRect(Entity* entity)
{
	const auto& collider = entity->Get<ColliderComponent>();
	const auto& transform = entity->Get<TransformComponent>();

	return SDL_Rect
	{
		.x = static_cast<int>(transform->x) + collider->m_offsetX,
		.y = static_cast<int>(transform->y) + collider->m_offsetY,
		.w = collider->m_width,
		.h = collider->m_height
	};
}

void CollisionSystem::ResolveCollision(Entity* a, Entity* b, SDL_Rect& rectA, SDL_Rect& rectB)
{
	auto* colliderA = a->Get<ColliderComponent>();
	auto* colliderB = b->Get<ColliderComponent>();
	auto* transformA = a->Get<TransformComponent>();
	auto* transformB = b->Get<TransformComponent>();

	if (colliderA == nullptr || colliderB == nullptr || transformA == nullptr || transformB == nullptr)
	{
		return;
	}

	// Trigger overlap only (no physical push-out yet).
	if (colliderA->m_isTrigger || colliderB->m_isTrigger)
	{
		return;
	}

	// Static vs static: nothing to resolve.
	if (colliderA->m_isStatic && colliderB->m_isStatic)
	{
		return;
	}

	const int overlapX = std::min(rectA.x + rectA.w, rectB.x + rectB.w) - std::max(rectA.x, rectB.x);
	const int overlapY = std::min(rectA.y + rectA.h, rectB.y + rectB.h) - std::max(rectA.y, rectB.y);

	if (overlapX <= 0 || overlapY <= 0)
	{
		return;
	}

	const int centerA_X = rectA.x + rectA.w / 2;
	const int centerA_Y = rectA.y + rectA.h / 2;
	const int centerB_X = rectB.x + rectB.w / 2;
	const int centerB_Y = rectB.y + rectB.h / 2;

	if (overlapX < overlapY)
	{
		const float separation = (centerA_X < centerB_X) ? -static_cast<float>(overlapX) : static_cast<float>(overlapX);

		if (colliderA->m_isStatic)
		{
			transformB->x -= separation;
		}
		else if (colliderB->m_isStatic)
		{
			transformA->x += separation;
		}
		else
		{
			transformA->x += separation * 0.5f;
			transformB->x -= separation * 0.5f;
		}
	}
	else
	{
		const float separation = (centerA_Y < centerB_Y) ? -static_cast<float>(overlapY) : static_cast<float>(overlapY);

		if (colliderA->m_isStatic)
		{
			transformB->y -= separation;
		}
		else if (colliderB->m_isStatic)
		{
			transformA->y += separation;
		}
		else
		{
			transformA->y += separation * 0.5f;
			transformB->y -= separation * 0.5f;
		}
	}	
}
