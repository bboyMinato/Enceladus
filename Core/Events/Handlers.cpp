#include "Handlers.h"
#include <print>

void SetupInteractionHandlers(EventBus& eventBus, Registry& registry)
{
	eventBus.Subscribe<InteractionEvent>([&eventBus, &registry](const InteractionEvent& event)
		{
			auto* interactable = event.target.Get<Interactable>();

			if (interactable->oneShot && interactable->used)
			{				
				return;
			}

			switch (interactable->interactionType)
			{
				case InteractionType::Dialogue:			
					std::println("Triggering event with entity: {}", event.target.GetId());
					eventBus.Emit(DialogueEvent{ event.player, event.target, interactable->dialogueId });
					break;
				
				default:
					break;
			}
		});
}

std::optional<Entity> FindClosestInteractable(Entity player, Registry& registry)
{
	const TransformComponent* playerTransform = player.Get<TransformComponent>();

	if (!playerTransform)
	{
		return std::nullopt;
	}

	std::optional<Entity> closestEntity = std::nullopt;
	float closestDistance = std::numeric_limits<float>::max();

	registry.ForEach<Interactable, TransformComponent>(
		[&](Entity entity, const Interactable& interactable, const TransformComponent& transform)
		{
			if (interactable.used && interactable.oneShot)
			{
				return; // Skip used interactables
			}

			float distance = CalculateDistance(*playerTransform, transform);
			if (distance < closestDistance && distance <= interactable.interactionDistance)
			{
				closestDistance = distance;
				closestEntity = entity;
			}
		}
	);

	return closestEntity;
}

float CalculateDistance(const TransformComponent& a, const TransformComponent& b)
{
	float dx = b.x - a.x;
	float dy = b.y - a.y;

	return std::sqrt(dx * dx + dy * dy);
}
