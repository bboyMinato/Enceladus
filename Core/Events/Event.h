#pragma once
#include "../ECS/Entity.h"
#include <string>

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