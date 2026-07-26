#pragma once
#include <optional>
#include "EventBus.h"
#include "../ECS/Registry.h"
#include "../ECS/TransformComponent.h"

class DialogueManager;

void SetupInteractionHandlers(
	EventBus& eventBus,
	Registry& registry,
	DialogueManager& dialogueManager);

std::optional<Entity> FindClosestInteractable(Entity player, Registry& registry);

float CalculateDistance(const TransformComponent& a, const TransformComponent& b);
