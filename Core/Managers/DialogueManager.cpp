#include "DialogueManager.h"
#include <nlohmann/json.hpp>
#include <fstream>

using Json = nlohmann::json;

bool DialogueManager::LoadDialogue(const std::filesystem::path& filePath)
{
	std::ifstream file(filePath);
	
	if (!file.is_open()) 
	{
		return false;
	}

	Json document = Json::parse(file, nullptr, false);

	if (document.is_discarded())
	{
		return false;
	}

	if (!document.is_object() ||
		!document.contains("id") ||
		!document.contains("nodes") ||
		!document["id"].is_string() ||
		!document["nodes"].is_object())
	{
		return false;
	}

	Dialogue dialogue;
	dialogue.m_id = document["id"].get<std::string>();

	if (dialogue.m_id.empty())
	{
		return false;
	}

	const Json& nodesJson = document["nodes"];

	for (const auto& [nodeId, nodeJson] : nodesJson.items())
	{
		if (nodeId.empty() || !nodeJson.is_object())
		{
			return false;
		}

		DialogueNode node;

		if (nodeJson.contains("nextNode"))
		{
			if (!nodeJson["nextNode"].is_string())
			{
				return false;
			}

			node.m_nextNode = nodeJson["nextNode"].get<std::string>();
		}

		if (nodeJson.contains("entries"))
		{
			const Json& entriesJson = nodeJson["entries"];

			if (!entriesJson.is_array())
			{
				return false;
			}

			for (const auto& entryJson : entriesJson)
			{
				if (!entryJson.is_object() || 
					!entryJson.contains("name") || 
					!entryJson.contains("speech") ||
					!entryJson["name"].is_string() ||
					!entryJson["speech"].is_string())
				{
					return false;
				}

				node.m_entries.push_back(
					{ 
						.m_name = entryJson["name"].get<std::string>(), 
						.m_speech = entryJson["speech"].get<std::string>() 
					});
			}			
		}

		if (nodeJson.contains("options"))
		{
			const Json& optionsJson = nodeJson["options"];
			if (!optionsJson.is_array())
			{
				return false;
			}

			for (const auto& optionJson : optionsJson)
			{
				if (!optionJson.is_object() ||
					!optionJson.contains("text") ||
					!optionJson.contains("target") ||
					!optionJson["text"].is_string() ||
					!optionJson["target"].is_string())
				{
					return false;
				}

				DialogueOption option
				{
					.m_text = optionJson["text"].get<std::string>(),
					.m_targetNode = optionJson["target"].get<std::string>()
				};

				if (option.m_text.empty() || option.m_targetNode.empty())
				{
					return false;
				}

				node.m_options.push_back(option);
			}
		}

		dialogue.m_nodes.emplace(nodeId, std::move(node));
	}

	if (!dialogue.m_nodes.contains("start"))
	{
		return false;
	}

	for (const auto& [nodeId, node] : dialogue.m_nodes)
	{
		if (!node.m_nextNode.empty() && !dialogue.m_nodes.contains(node.m_nextNode))
		{
			return false;
		}

		for (const auto& option : node.m_options)
		{
			if (!dialogue.m_nodes.contains(option.m_targetNode))
			{
				return false;
			}
		}
	}

	m_dialogues.insert_or_assign(dialogue.m_id, std::move(dialogue));

	return true;
}