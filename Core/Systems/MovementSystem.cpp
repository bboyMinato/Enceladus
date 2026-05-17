#include "MovementSystem.h"
#include "CollisionSystem.h"
#include "../ECS/ColliderComponent.h"

void MovementSystem::Update(Registry& registry, float deltaTime)
{
	registry.ForEach<TransformComponent, MovementComponent>(
		[&](Entity entity, TransformComponent& transform, const MovementComponent& movement)
		{
			ApplyMovement(entity, registry, deltaTime);
		}
	);
}

void MovementSystem::ApplyMovement(Entity& entity, Registry& registry, float deltaTime)
{
	auto& transform = *entity.Get<TransformComponent>();
	auto& movement = *entity.Get<MovementComponent>();

	float velocityX = movement.velocity.x;
	float velocityY = movement.velocity.y;

	if (velocityX != 0.0f || velocityY != 0.0f)
	{
		const float magnitude = std::sqrt((velocityX * velocityX) + (velocityY * velocityY));
		
		if (magnitude > 0.0001f)
		{
			velocityX /= magnitude;
			velocityY /= magnitude;
		}
	}

	transform.x += velocityX * movement.moveSpeed * deltaTime;
	//ResolveCollisionOnX(entity, registry);

	transform.y += velocityY * movement.moveSpeed * deltaTime;
	//ResolveCollisionOnY(entity, registry);
}

SDL_Rect MovementSystem::GetRect(Entity& entity)
{
	const auto& transform = entity.Get<TransformComponent>();
	const auto& collider = entity.Get<ColliderComponent>();

	return SDL_Rect
	{
		static_cast<int>(transform->x + collider->m_offsetX),
		static_cast<int>(transform->y + collider->m_offsetY),
		collider->m_width,
		collider->m_height
	};
}

void MovementSystem::ResolveCollisionOnX(Entity& entity, Registry& registry)
{
	auto rectA = GetRect(entity);

	auto entities = registry.GetEntitiesWithComponents<ColliderComponent, TransformComponent>();

	for (auto& other : entities)
	{
		if (other == entity)
		{
			continue;
		}

		auto& colliderB = *other.Get<ColliderComponent>();
		if (colliderB.m_isStatic)
		{
			continue;
		}
		
		auto rectB = GetRect(other);
		
		auto& movement = *entity.Get<MovementComponent>();
	
		if (SDL_HasIntersection(&rectA, &rectB))
		{
			auto& transformA = *entity.Get<TransformComponent>();
			auto& colliderA = *entity.Get<ColliderComponent>();
			
			if (rectA.x < rectB.x)
			{
				transformA.x = rectB.x - rectA.w - colliderA.m_offsetX;
			}
			else
			{
				transformA.x = rectB.x + rectB.w - colliderA.m_offsetX;
			}
		}

		movement.velocity.x = 0.0f;

		rectA = GetRect(entity);
	}
}

void MovementSystem::ResolveCollisionOnY(Entity& entity, Registry& registry)
{
	auto rectA = GetRect(entity);
	auto entities = registry.GetEntitiesWithComponents<ColliderComponent, TransformComponent>();

	for (auto& other : entities)
	{
		if (other == entity)
		{
			continue;
		}

		auto& colliderB = *other.Get<ColliderComponent>();
		if (colliderB.m_isStatic)
		{
			continue;
		}

		auto rectB = GetRect(other);
		
		auto& movement = *entity.Get<MovementComponent>();

		if (SDL_HasIntersection(&rectA, &rectB))
		{
			auto& transformA = *entity.Get<TransformComponent>();
			auto& colliderA = *entity.Get<ColliderComponent>();
			
			if (rectA.y < rectB.y)
			{
				transformA.y = rectB.y - rectA.h - colliderA.m_offsetY;
			}
			else
			{
				transformA.y = rectB.y + rectB.h - colliderA.m_offsetY;
			}
		}

		movement.velocity.y = 0.0f;

		rectA = GetRect(entity);
	}
}
