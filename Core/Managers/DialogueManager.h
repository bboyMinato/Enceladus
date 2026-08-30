#pragma once
#include "../Utility/Dialogue.h"

#include <filesystem>
#include <string>
#include <unordered_map>

class DialogueManager
{
public:
	DialogueManager() = default;

	bool LoadDialogue(const std::filesystem::path& filePath);

	const Dialogue* FindDialogue(const std::string& dialogueId) const
	{
		const auto iterator = m_dialogues.find(dialogueId);
		return iterator == m_dialogues.end() ? nullptr : &iterator->second;
	}

	const Dialogue& GetDialogue(const std::string& dialogueId) const { return m_dialogues.at(dialogueId); }

private:
    std::unordered_map<std::string, Dialogue> m_dialogues;
};