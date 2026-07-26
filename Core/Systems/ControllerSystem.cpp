#include "ControllerSystem.h"
#include <optional>
#include "../Events/Handlers.h"

bool ControllerSystem::PopState(Registry& registry, const InputSystem& input)
{
	bool wantsBack = false; 

	registry.ForEach<ControllerComponent>(
		[&](Entity entity, const ControllerComponent& controller)
		{
			if (controller.isEnabled && input.WasKeyPressed(controller.backKey))
			{
				wantsBack = true;
			}
		}
	);

	return wantsBack;
}

void ControllerSystem::Update(Registry& registry, const InputSystem& input, EventBus& eventBus)
{
	registry.ForEach<ControllerComponent, MovementComponent>(
		[&](Entity entity, const ControllerComponent& controller, MovementComponent& movement)
		{
			UpdateMovement(input, controller, movement);
			HandleInteraction(registry, input, controller, entity, eventBus);
		}
	);
}

void ControllerSystem::UpdateMovement(const InputSystem& input, const ControllerComponent& controller, MovementComponent& movement)
{
#ifdef _DEBUG
	if (movement.m_useManualMovement)
	{
		return;
	}
#endif

	movement.velocity = {};

	if (!controller.isEnabled)
	{
		return;
	}

	if (IsDown(input, controller.moveLeftPrimary, controller.moveLeftSecondary))
	{
		movement.velocity.x -= 1.0f;
	}

	if (IsDown(input, controller.moveRightPrimary, controller.moveRightSecondary))
	{
		movement.velocity.x += 1.0f;
	}

	if (IsDown(input, controller.moveUpPrimary, controller.moveUpSecondary))
	{
		movement.velocity.y -= 1.0f;
	}

	if (IsDown(input, controller.moveDownPrimary, controller.moveDownSecondary))
	{
		movement.velocity.y += 1.0f;
	}
}

void ControllerSystem::HandleInteraction(Registry& registry, const InputSystem& input, const ControllerComponent& controller, Entity entity, EventBus& eventBus)
{
	if (!controller.isEnabled)
	{
		return;
	}

	if (input.WasKeyPressed(controller.interactPrimary) || input.WasKeyPressed(controller.interactSecondary))
	{
		std::optional<Entity> closestInteractable = FindClosestInteractable(entity, registry);

		if (closestInteractable.has_value())
		{
			InteractionEvent event{ entity, closestInteractable.value() };
			eventBus.Emit(event);
		}
	}
}

bool ControllerSystem::IsDown(const InputSystem& input, SDL_Scancode primary, SDL_Scancode secondary)
{
	return input.IsKeyDown(primary) || input.IsKeyDown(secondary);
}