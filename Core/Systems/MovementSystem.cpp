#include "MovementSystem.h"

void MovementSystem::Update(Registry& registry, float deltaTime)
{
	registry.ForEach<TransformComponent, MovementComponent>(
		[&](Entity entity, TransformComponent& transform, const MovementComponent& movement)
		{
			ApplyMovement(transform, movement, deltaTime);
		}
	);
}

void MovementSystem::ApplyMovement(TransformComponent& transform, const MovementComponent& movement, float deltaTime)
{
	float velocityX = movement.velocity.x;
	float velocityY = movement.velocity.y;

	if (velocityX != 0.0f || velocityY != 0.0f)
	{
		const float magnitude = std::sqrt((velocityX * velocityX) + (velocityY * velocityY));
		velocityX /= magnitude;
		velocityY /= magnitude;
	}

	transform.x += velocityX * movement.moveSpeed * deltaTime;
	transform.y += velocityY * movement.moveSpeed * deltaTime;
}