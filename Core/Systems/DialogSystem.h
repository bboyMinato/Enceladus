#pragma once

#include "../ECS/DialogSegmentComponent.h"
#include "../ECS/Registry.h"


class DialogManager;
class RenderSystem;
class InputSystem;
class Registry;

class DialogSystem
{
public:
	static void Update(const Entity& player, const InputSystem& input, Registry& registry, const DialogManager& dialogManager);
	static void UpdateText(DialogSegmentComponent& segmentComponent, const DialogManager& dialogManager);
};
