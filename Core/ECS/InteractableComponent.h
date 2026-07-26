#pragma once
#include <string>

enum class InteractionType
{
	Dialogue,
	Open,
	Activate,
	Pickup,
	Examine,
	Default
};

struct InteractableComponent
{
	InteractionType interactionType{};
	float interactionDistance{ 100.0f };
	bool requiresKey{ true };
	bool oneShot{ false };
	bool used{ false };
	std::string dialogueId;
};