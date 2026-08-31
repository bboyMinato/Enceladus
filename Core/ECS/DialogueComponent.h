#pragma once
#include <string>

struct DialogueComponent
{
	std::string dialogueId;
	std::string startNodeId{ "start" };
};