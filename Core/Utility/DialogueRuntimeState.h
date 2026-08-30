#pragma once

#include "Dialogue.h"

#include <cstddef>
#include <string>

struct DialogueRuntimeState
{
	const Dialogue* m_currentDialogue{ nullptr };

	std::string m_currentNode;
	std::size_t m_currentEntryIndex{ 0 };
	std::size_t m_selectedOptionIndex{ 0 };

	bool m_isActive{ false };
	bool m_showingOptions{ false };

	bool HasDialogue() const
	{
		return m_isActive && GetCurrentNode() != nullptr;
	}

	const DialogueNode* GetCurrentNode() const
	{
		if (!m_currentDialogue)
		{
			return nullptr;
		}

		const auto it = m_currentDialogue->m_nodes.find(m_currentNode);
		if (it != m_currentDialogue->m_nodes.end())
		{
			return &it->second;
		}

		return nullptr;
	}

	const DialogueEntry* GetCurrentEntry() const
	{
		const DialogueNode* node = GetCurrentNode();
		if (!node || m_currentEntryIndex >= node->m_entries.size())
		{
			return nullptr;
		}

		return &node->m_entries[m_currentEntryIndex];
	}

	std::string GetVisibleText() const
	{
		const DialogueEntry* entry = GetCurrentEntry();
		return entry ? entry->m_speech : "";
	}
};