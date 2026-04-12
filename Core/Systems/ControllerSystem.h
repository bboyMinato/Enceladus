#pragma once
#include "../ECS/MovementComponent.h"
#include "../ECS/ControllerComponent.h"
#include "../ECS/Registry.h"
#include "InputSystem.h"

class ControllerSystem final
{
public:
	static bool PopState(Registry& registry, const InputSystem& input);
	static void Update(Registry& registry, const InputSystem& input);

private:
	static void UpdateMovement(const InputSystem& input, const ControllerComponent& controller, MovementComponent& movement);
	static bool IsDown(const InputSystem& input, SDL_Scancode primary, SDL_Scancode secondary);
};