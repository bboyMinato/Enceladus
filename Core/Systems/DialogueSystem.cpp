#include "DialogueSystem.h"
#include "../Utility/Dialogue.h"
#include "../Utility/DialogueRuntimeState.h"
#include "../Systems/InputSystem.h"
#include <SDL2/SDL.h>

bool DialogueSystem::Start(DialogueRuntimeState& state, const Dialogue& dialogue, const std::string& startNode)
{
	if (state.m_isActive || startNode.empty() || !dialogue.m_nodes.contains(startNode))
	{
		return false;
	}

	state.m_currentDialogue = &dialogue;
	state.m_currentNode = startNode;
	state.m_currentEntryIndex = 0;
	state.m_selectedOptionIndex = 0;
	state.m_isActive = true;
	state.m_showingOptions = false;

	return true;
}

void DialogueSystem::Update(DialogueRuntimeState& state, const InputSystem& input)
{
	if (!state.m_isActive)
	{
		return;
	}

	const DialogueNode* currentNode = state.GetCurrentNode();
	if (!currentNode)
	{
		Close(state);
		return;
	}

	if (input.WasKeyPressed(SDL_SCANCODE_ESCAPE))
	{
		Close(state);
		return;
	}

	if (state.m_showingOptions)
	{
		if (currentNode->m_options.empty())
		{
			Close(state);
			return;
		}

		if (input.WasKeyPressed(SDL_SCANCODE_UP) && state.m_selectedOptionIndex > 0)
		{
			--state.m_selectedOptionIndex;
		}
		else if (input.WasKeyPressed(SDL_SCANCODE_DOWN) && state.m_selectedOptionIndex + 1 < currentNode->m_options.size())
		{
			++state.m_selectedOptionIndex;
		}

		if (input.WasKeyPressed(SDL_SCANCODE_RETURN))
		{
			SelectOption(state, state.m_selectedOptionIndex);
		}

		return;
	}

	if (input.WasKeyPressed(SDL_SCANCODE_RETURN))
	{
		Advance(state);
	}
}

void DialogueSystem::Advance(DialogueRuntimeState& state)
{
	const DialogueNode* node = state.GetCurrentNode();
	if (!node)
	{
		Close(state);
		return;
	}

	if (state.m_currentEntryIndex + 1 < node->m_entries.size())
	{
		++state.m_currentEntryIndex;
		return;
	}

	if (!node->m_options.empty())
	{
		state.m_showingOptions = true;
		state.m_selectedOptionIndex = 0;
		return;
	}

	if (node->m_nextNode.empty())
	{
		Close(state);
		return;
	}

	const auto nextNodeIterator = state.m_currentDialogue->m_nodes.find(node->m_nextNode);
	if (nextNodeIterator == state.m_currentDialogue->m_nodes.end())
	{
		Close(state);
		return;
	}

	state.m_currentNode = node->m_nextNode;
	state.m_currentEntryIndex = 0;
	state.m_selectedOptionIndex = 0;
	state.m_showingOptions = false;
}

void DialogueSystem::SelectOption(DialogueRuntimeState& state, std::size_t optionIndex)
{
	const DialogueNode* node = state.GetCurrentNode();
	if (!node || optionIndex >= node->m_options.size())
	{
		Close(state);
		return;
	}

	const std::string targetNode = node->m_options[optionIndex].m_targetNode;
	
	const auto targetNodeIterator = state.m_currentDialogue->m_nodes.find(targetNode);
	if (targetNodeIterator == state.m_currentDialogue->m_nodes.end())
	{
		Close(state);
		return;
	}

	state.m_currentNode = targetNode;
	state.m_currentEntryIndex = 0;
	state.m_selectedOptionIndex = 0;
	state.m_showingOptions = false;
}

void DialogueSystem::Close(DialogueRuntimeState& state)
{
	state.m_currentDialogue = nullptr;
	state.m_currentNode.clear();
	state.m_currentEntryIndex = 0;
	state.m_selectedOptionIndex = 0;
	state.m_isActive = false;
	state.m_showingOptions = false;
}