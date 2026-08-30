#pragma once
#include <filesystem>
#include <vector>
#include <string>
#include <unordered_map>

struct DialogueEntry 
{
    std::string m_name;
    std::string m_speech;
};

struct DialogueOption
{
	std::string m_text;
	std::string m_targetNode;
};

struct DialogueNode
{
	std::string m_nextNode;

	std::vector<DialogueEntry> m_entries;
	std::vector<DialogueOption> m_options;
};

struct Dialogue
{
	std::string m_id;
	std::unordered_map<std::string, DialogueNode> m_nodes;
};