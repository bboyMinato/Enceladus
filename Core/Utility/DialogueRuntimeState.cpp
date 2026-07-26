#include "DialogueRuntimeState.h"

#include "../Managers/DialogManager.h"

#include <algorithm>

namespace
{
	constexpr float defaultCharactersPerSecond = 32.0f;
}

void DialogueRuntimeState::SetDialogue(const Dialogue* dialogue, std::string dialogueId)
{
	currentDialogue = dialogue;
	currentDialogueId = std::move(dialogueId);
	currentSegmentIndex = 0;
	currentCharacterIndex = 0;
	characterTimer = 0.0f;
	isTyping = false;
	waitingForAdvance = false;

	if (!currentDialogue)
	{
		return;
	}

	const auto& entries = currentDialogue->GetEntries();
	if (entries.empty())
	{
		waitingForAdvance = true;
		return;
	}

	const auto& speech = entries.front().GetSpeech();
	if (speech.empty())
	{
		waitingForAdvance = true;
		return;
	}

	currentCharacterIndex = 1;
	isTyping = currentCharacterIndex < speech.size();
	waitingForAdvance = !isTyping && currentCharacterIndex >= speech.size();
}

void DialogueRuntimeState::Clear()
{
	currentDialogue = nullptr;
	currentDialogueId.clear();
	currentSegmentIndex = 0;
	currentCharacterIndex = 0;
	characterTimer = 0.0f;
	isTyping = false;
	waitingForAdvance = false;
}

bool DialogueRuntimeState::HasDialogue() const
{
	return currentDialogue != nullptr;
}

void DialogueRuntimeState::Update(float deltaTime)
{
	if (!currentDialogue || waitingForAdvance)
	{
		return;
	}

	const auto& entries = currentDialogue->GetEntries();
	if (currentSegmentIndex >= entries.size())
	{
		Clear();
		return;
	}

	const auto& speech = entries[currentSegmentIndex].GetSpeech();
	if (speech.empty())
	{
		waitingForAdvance = true;
		isTyping = false;
		currentCharacterIndex = 0;
		return;
	}

	if (currentCharacterIndex >= speech.size())
	{
		isTyping = false;
		waitingForAdvance = true;
		return;
	}

	isTyping = true;
	characterTimer += deltaTime;

	const float charactersPerSecond = defaultCharactersPerSecond;
	const float secondsPerCharacter = 1.0f / charactersPerSecond;

	while (characterTimer >= secondsPerCharacter && currentCharacterIndex < speech.size())
	{
		characterTimer -= secondsPerCharacter;
		++currentCharacterIndex;
	}

	if (currentCharacterIndex >= speech.size())
	{
		currentCharacterIndex = speech.size();
		isTyping = false;
		waitingForAdvance = true;
	}
}

bool DialogueRuntimeState::Advance()
{
	if (!currentDialogue)
	{
		return false;
	}

	const auto& entries = currentDialogue->GetEntries();
	if (entries.empty() || currentSegmentIndex >= entries.size())
	{
		Clear();
		return false;
	}

	const auto& speech = entries[currentSegmentIndex].GetSpeech();
	if (currentCharacterIndex < speech.size())
	{
		currentCharacterIndex = speech.size();
		isTyping = false;
		waitingForAdvance = true;
		characterTimer = 0.0f;
		return true;
	}

	if (currentSegmentIndex + 1 < entries.size())
	{
		++currentSegmentIndex;
		const auto& nextSpeech = entries[currentSegmentIndex].GetSpeech();
		currentCharacterIndex = nextSpeech.empty() ? 0 : 1;
		characterTimer = 0.0f;
		isTyping = !nextSpeech.empty() && currentCharacterIndex < nextSpeech.size();
		waitingForAdvance = !isTyping && currentCharacterIndex >= nextSpeech.size();
		return true;
	}

	Clear();
	return false;
}

std::string DialogueRuntimeState::GetVisibleText() const
{
	if (!currentDialogue)
	{
		return {};
	}

	const auto& entries = currentDialogue->GetEntries();
	if (currentSegmentIndex >= entries.size())
	{
		return {};
	}

	const auto& speech = entries[currentSegmentIndex].GetSpeech();
	const std::size_t visibleCharacters = std::min(currentCharacterIndex, speech.size());
	return speech.substr(0, visibleCharacters);
}
