#pragma once
#include <filesystem>
#include <vector>

class DialogueEntry;

class Dialogue {
public:
    Dialogue(const std::filesystem::path &path);
    const std::vector<DialogueEntry>& GetEntries() const;

private:
    std::vector<DialogueEntry> m_entries;
};

class DialogueEntry {
public:
    DialogueEntry(const std::string_view &chunk);
    const std::string &GetName() const;
    const std::string &GetSpeech() const;

private:
    std::string m_name;
    std::string m_speech;
};
