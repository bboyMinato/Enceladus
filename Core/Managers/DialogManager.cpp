#include "DialogManager.h"

#include <filesystem>
#include <fstream>
#include <ranges>

bool DialogManager::Init()
{
	for (const auto& entry : std::filesystem::recursive_directory_iterator(dialogPath))
	{
		const auto& path = entry.path();
		if (path.extension() == ".dialog")
		{
			const auto& filename = path.filename();
			m_dialogs.emplace(filename.string(), Dialog(path));
		}
	}
	return true;
}

const Dialog& DialogManager::GetDialog(const std::string& key) const
{
	return m_dialogs.at(key);
}

Dialog::Dialog(const std::filesystem::path& path)
{
	std::ifstream inputFile(path);

	if (!inputFile)
	{
		throw std::runtime_error(std::format("Failed to open file '%s'.", path.string()));
	}

	std::stringstream buffer;
	buffer << inputFile.rdbuf();

	std::string lines = buffer.str();

	constexpr std::string_view delimiter = "\n\n";
	for (const auto& chunk : std::views::split(lines, delimiter))
	{
		const auto substr = std::string_view(chunk);
		const auto entry = DialogEntry(substr);
		m_entries.push_back(entry);
	}
}

DialogEntry::DialogEntry(const std::string_view& chunk)
{
	constexpr std::string_view delimiter = ": ";
	const auto index = chunk.find(delimiter);
	m_name = std::string(chunk.substr(0, index));
	m_speech = std::string(chunk.substr(index + delimiter.length()));
}

const std::string& DialogEntry::GetName()
{
	return m_name;
}

const std::string& DialogEntry::GetSpeech()
{
	return m_speech;
}
