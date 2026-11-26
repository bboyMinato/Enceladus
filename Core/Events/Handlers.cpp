#include "Handlers.h"
#include <cmath>
#include "../Managers/DialogueManager.h"
#include "../ECS/InteractableComponent.h"
#include "../Systems/DialogueSystem.h"
#include "../Utility/DialogueRuntimeState.h"
#include "../ECS/DialogueComponent.h"

void SetupInteractionHandlers(EventBus& eventBus, DialogueManager& dialogueManager, DialogueRuntimeState& dialogueState)
{
    eventBus.Subscribe<InteractionEvent>([&eventBus, &dialogueState](const InteractionEvent &event)
    {
        const auto* interactable = event.target.Get<InteractableComponent>();
		const auto* dialogue = event.target.Get<DialogueComponent>();

		if (!interactable || !dialogue ||
            (interactable->oneShot && interactable->used) ||
            dialogueState.m_isActive)
        {
            return;
        }

		eventBus.Emit(DialogueEvent{ event.player, event.target, dialogue->dialogueId, dialogue->startNodeId });
    });

    eventBus.Subscribe<DialogueEvent>(
        [&dialogueManager, &dialogueState](const DialogueEvent& event)
        {
            const Dialogue* dialogue = dialogueManager.FindDialogue(event.dialogueId);
            if (!dialogue)
            {
                return;
            }

            Entity target = event.npc;
            auto* dialogueComponent = target.Get<DialogueComponent>();

            if (dialogueComponent && !dialogueComponent->repeatNodeId.empty() &&
                !dialogue->m_nodes.contains(dialogueComponent->repeatNodeId))
            {
                return;
            }

            if (!DialogueSystem::Start(dialogueState, *dialogue, event.startNodeId))
            {
                return;
            }

            if (dialogueComponent && !dialogueComponent->repeatNodeId.empty())
            {
                dialogueComponent->startNodeId = dialogueComponent->repeatNodeId;
            }

            if (auto* interactable = target.Get<InteractableComponent>();
                interactable && interactable->oneShot)
            {
                interactable->used = true;
            }
        });
}

std::optional<Entity> FindClosestInteractable(Entity player, Registry& registry)
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

float CalculateDistance(const TransformComponent& a, const TransformComponent& b)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;

    return std::sqrt(dx * dx + dy * dy);
}