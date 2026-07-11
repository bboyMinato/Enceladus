#pragma once
#include <filesystem>
#include <map>
#include <string>
#include <vector>

class DialogEntry;
class Dialogue;

const auto dialogPath = std::filesystem::path("Assets/dialogs");

class DialogueManager {
public:
    DialogueManager() = default;

    bool Init();
    const Dialogue *GetDialogue(const std::string &key) const;

private:
    std::map<std::string, Dialogue> m_dialogs;
};

class Dialogue {
public:
    Dialogue(const std::filesystem::path &path);
    const std::vector<DialogEntry> GetEntries() const;

private:
    std::vector<DialogEntry> m_entries;
};

class DialogEntry {
public:
    DialogEntry(const std::string_view &chunk);
    const std::string &GetName() const;
    const std::string &GetSpeech() const;

private:
    std::string m_name;
    std::string m_speech;
};