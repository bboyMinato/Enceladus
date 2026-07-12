#include "DialogManager.h"

#include <SDL_log.h>
#include <filesystem>
#include <format>
#include <fstream>
#include <system_error>

bool DialogueManager::Init()
{
    std::error_code ec;

    if (!std::filesystem::exists(dialogPath, ec))
    {
        SDL_Log("Dialogue directory does not exist: %s", dialogPath.string().c_str());
        return false;
    }

    for (std::filesystem::recursive_directory_iterator it(
             dialogPath,
             std::filesystem::directory_options::skip_permission_denied,
             ec), end;
         it != end;
         it.increment(ec))
    {
        if (ec)
        {
            SDL_Log("Failed to iterate dialogue directory '%s': %s", dialogPath.string().c_str(), ec.message().c_str());
            ec.clear();
            continue;
        }

        const auto& entry = *it;
        if (!entry.is_regular_file(ec) || ec)
        {
            ec.clear();
            continue;
        }

        const auto& path = entry.path();
        if (path.extension() == ".dialog")
        {
            const auto& stem = path.stem();
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

    std::string chunk;
    std::string line;

    while (std::getline(inputFile, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }

        if (line.empty())
        {
            if (!chunk.empty())
            {
                m_entries.emplace_back(chunk);
                chunk.clear();
            }
            continue;
        }

        if (!chunk.empty())
        {
            chunk.push_back('\n');
        }
        chunk += line;
    }

    if (!chunk.empty())
    {
        m_entries.emplace_back(chunk);
    }
}

const std::vector<DialogEntry>& Dialogue::GetEntries() const {
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
