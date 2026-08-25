#pragma once
#include <string>
#include <nlohmann/json.hpp>

enum class InteractionType
{
	Default,
	Dialogue,
	Open,
	Activate,
	Pickup,
	Examine
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

NLOHMANN_JSON_SERIALIZE_ENUM(InteractionType,
{
	{ InteractionType::Default,  "default" },
	{ InteractionType::Open,     "open" },
	{ InteractionType::Pickup,   "pickup" },
	{ InteractionType::Activate, "activate" },
	{ InteractionType::Dialogue, "dialogue" },
	{ InteractionType::Examine,  "examine" }
})