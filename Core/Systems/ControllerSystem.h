#pragma once
#include "../ECS/MovementComponent.h"
#include "../ECS/ControllerComponent.h"
#include "../ECS/Registry.h"
#include "../Events/EventBus.h"
#include "../Utility/DialogueRuntimeState.h"
#include "InputSystem.h"

class ControllerSystem final
{
public:
	static bool PopState(Registry& registry, const InputSystem& input);
	static void Update(Registry& registry, const InputSystem& input, EventBus& eventBus, DialogueRuntimeState& dialogueState);

private:
	static void UpdateMovement(const InputSystem& input, const ControllerComponent& controller, MovementComponent& movement, const DialogueRuntimeState& dialogueState);
	static void HandleInteraction(Registry& registry, const InputSystem& input, const ControllerComponent& controller, Entity entity, EventBus& eventBus, DialogueRuntimeState& dialogueState);
	static bool IsDown(const InputSystem& input, SDL_Scancode primary, SDL_Scancode secondary);
};
