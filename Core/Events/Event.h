#pragma once
#include "../ECS/Entity.h"
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

struct Interactable
{
	InteractionType interactionType{};
	float interactionDistance{ 100.0f };
	bool requiresKey{ true };
	bool oneShot{ false };
	bool used{ false };
	std::string dialogueId;
};

struct TriggerEnterEvent
{
	Entity triggerEntity;
	Entity otherEntity;
};

struct TriggerExitEvent
{
	Entity triggerEntity;
	Entity otherEntity;
};

struct InteractionEvent
{
	Entity player;
	Entity target;
};

struct DialogueEvent
{
	Entity player;
	Entity npc;
	std::string dialogueId;
};