#include "Handlers.h"
#include "../Managers/DialogManager.h"
#include "../ECS/InteractableComponent.h"
#include <cmath>
#include <print>

void SetupInteractionHandlers(EventBus &eventBus, Registry &registry, DialogueManager &dialogueManager)
{
    eventBus.Subscribe<InteractionEvent>([&eventBus, &registry](const InteractionEvent& event) 
    {
         auto* interactable = event.target.Get<InteractableComponent>();
    });

    eventBus.Subscribe<InteractionEvent>([&eventBus, &dialogueManager](const InteractionEvent &event) 
    {
        auto *interactable = event.target.Get<InteractableComponent>();

        if (interactable->oneShot && interactable->used)
        {
            return;
        }

        switch (interactable->interactionType)
        {
        case InteractionType::Dialogue:
            if (dialogueManager.HasActiveDialogue())
            {
                return;
            }

            eventBus.Emit(DialogueEvent{event.player, event.target, interactable->dialogueId});
            break;

        case InteractionType::Open:
            throw std::logic_error("Open interaction not implemented yet.");
            break;

        case InteractionType::Activate:
            throw std::logic_error("Activate interaction not implemented yet.");
            break;

        case InteractionType::Pickup:
            throw std::logic_error("Pickup interaction not implemented yet.");
            break;

        case InteractionType::Examine:
            throw std::logic_error("Examine interaction not implemented yet.");
            break;

        default:
            break;
        }
    });

    eventBus.Subscribe<DialogueEvent>([&dialogueManager](const DialogueEvent &event) {
        dialogueManager.SetDialogue(event.dialogueId);
    });
}

std::optional<Entity> FindClosestInteractable(Entity player, Registry &registry)
{
    const TransformComponent *playerTransform = player.Get<TransformComponent>();

    if (!playerTransform)
    {
        return std::nullopt;
    }

    std::optional<Entity> closestEntity = std::nullopt;
    float closestDistance = std::numeric_limits<float>::max();

	registry.ForEach<InteractableComponent, TransformComponent>(
		[&](Entity entity, const InteractableComponent& interactable, const TransformComponent& transform)
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
        });

    return closestEntity;
}

float CalculateDistance(const TransformComponent &a, const TransformComponent &b)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;

    return std::sqrt(dx * dx + dy * dy);
}
