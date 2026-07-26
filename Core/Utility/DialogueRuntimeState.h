#pragma once

#include "Dialogue.h"

#include <cstddef>
#include <string>

struct DialogueRuntimeState
{
	const Dialogue* currentDialogue{ nullptr };
	std::string currentDialogueId;
	std::size_t currentSegmentIndex{ 0 };
	std::size_t currentCharacterIndex{ 0 };
	float characterTimer{ 0.0f };
	bool isTyping{ false };
	bool waitingForAdvance{ false };

	void SetDialogue(const Dialogue* dialogue, std::string dialogueId = {});
	void Clear();
	void Update(float deltaTime);
	[[nodiscard]] bool Advance();
	[[nodiscard]] bool HasDialogue() const;
	[[nodiscard]] std::string GetVisibleText() const;
};
