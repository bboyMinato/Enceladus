#include "Dialogue.h"

#include <fstream>

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

const std::vector<DialogueEntry>& Dialogue::GetEntries() const {
    return m_entries;
}

DialogueEntry::DialogueEntry(const std::string_view &chunk) {
    constexpr std::string_view delimiter = ": ";
    const auto index = chunk.find(delimiter);
    m_name = std::string(chunk.substr(0, index));
    m_speech = std::string(chunk.substr(index + delimiter.length()));
}

const std::string &DialogueEntry::GetName() const {
    return m_name;
}

const std::string &DialogueEntry::GetSpeech() const {
    return m_speech;
}
