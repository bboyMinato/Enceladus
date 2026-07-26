#pragma once
#include "../ECS/MovementComponent.h"
#include "../ECS/ControllerComponent.h"
#include "../ECS/Registry.h"
#include "../Events/EventBus.h"
#include "InputSystem.h"

class ControllerSystem final
{
public:
	static bool PopState(Registry& registry, const InputSystem& input);
	static void Update(Registry& registry, const InputSystem& input, EventBus& eventBus);

private:
	static void UpdateMovement(const InputSystem& input, const ControllerComponent& controller, MovementComponent& movement);
	static void HandleInteraction(Registry& registry, const InputSystem& input, const ControllerComponent& controller, Entity entity, EventBus& eventBus);
	static bool IsDown(const InputSystem& input, SDL_Scancode primary, SDL_Scancode secondary);
};
