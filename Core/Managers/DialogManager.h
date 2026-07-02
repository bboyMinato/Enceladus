#pragma once
#include <string>
#include <filesystem>
#include <list>
#include <map>

class DialogEntry;
class Dialog;

const auto dialogPath = std::filesystem::path("Assets/dialogs");

class DialogManager
{
public:
	DialogManager() = default;

	bool Init();
	const Dialog& GetDialog(const std::string& key) const;

private:
	std::map<std::string, Dialog> m_dialogs;
};

class Dialog
{
public:
	Dialog(const std::filesystem::path& path);


	
private:
	std::list<DialogEntry> m_entries;
};

class DialogEntry
{
public:
	DialogEntry(const std::string_view& chunk);
	const std::string& GetName();
	const std::string& GetSpeech();

private:
	std::string m_name;
	std::string m_speech;
};