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

	transform.y += velocityY * movement.moveSpeed * deltaTime;
}