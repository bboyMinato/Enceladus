#pragma once
#include <string>

struct InteractableComponent
{
	float interactionDistance{ 100.0f };
	bool requiresKey{ true };
	bool oneShot{ false };
	bool used{ false };
	std::string dialogueId;
};