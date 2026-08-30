#pragma once
#include <optional>
#include "EventBus.h"
#include "../ECS/Registry.h"
#include "../ECS/TransformComponent.h"

class DialogueManager;
class DialogueRuntimeState;

void SetupInteractionHandlers(EventBus& eventBus, DialogueManager& dialogueManager, DialogueRuntimeState& dialogueState);

std::optional<Entity> FindClosestInteractable(Entity player, Registry& registry);

float CalculateDistance(const TransformComponent& a, const TransformComponent& b);
