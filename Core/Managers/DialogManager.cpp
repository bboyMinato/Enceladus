#include "DialogManager.h"

#include <SDL_log.h>
#include <filesystem>
#include <format>
#include <fstream>
#include <ranges>

bool DialogueManager::Init() {
    for (const auto &entry : std::filesystem::recursive_directory_iterator(dialogPath)) {
        const auto &path = entry.path();
        if (path.extension() == ".dialog") {
            const auto &stem = path.stem();
            m_dialogs.emplace(stem.string(), Dialogue(path));
        }
    }
    return true;
}

const Dialogue *DialogueManager::GetDialogue(const std::string &key) const {
    auto it = m_dialogs.find(key);
    if (it == m_dialogs.end())
    {
        SDL_Log("Unknown dialogue id: '%s'", key.c_str());
        return nullptr;
    }
    return &it->second;
}

Dialogue::Dialogue(const std::filesystem::path &path) {
    std::ifstream inputFile(path);

    if (!inputFile) {
        throw std::runtime_error(std::format("Failed to open file '%s'.", path.string()));
    }

    std::stringstream buffer;
    buffer << inputFile.rdbuf();

    std::string lines = buffer.str();

    for (const auto &chunk : std::views::split(lines, "\n\n")) {
        const auto substr = std::string_view(chunk);
        const auto entry = DialogEntry(substr);
        m_entries.push_back(entry);
    }
}

const std::vector<DialogEntry> Dialogue::GetEntries() const {
    return m_entries;
}

DialogEntry::DialogEntry(const std::string_view &chunk) {
    constexpr std::string_view delimiter = ": ";
    const auto index = chunk.find(delimiter);
    m_name = std::string(chunk.substr(0, index));
    m_speech = std::string(chunk.substr(index + delimiter.length()));
}

const std::string &DialogEntry::GetName() const {
    return m_name;
}

const std::string &DialogEntry::GetSpeech() const {
    return m_speech;
}