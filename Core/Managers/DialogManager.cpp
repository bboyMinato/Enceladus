#include "DialogManager.h"

#include <SDL2/SDL_log.h>
#include <filesystem>
#include <system_error>

bool DialogueManager::Init()
{
    std::error_code errorCode;

    if (!std::filesystem::exists(dialogPath, errorCode))
    {
        SDL_Log("Dialogue directory does not exist: %s", dialogPath.string().c_str());
        return false;
    }

    for (std::filesystem::recursive_directory_iterator it(
             dialogPath,
             std::filesystem::directory_options::skip_permission_denied,
             errorCode), end;
         it != end;
         it.increment(errorCode))
    {
        if (errorCode)
        {
            SDL_Log("Failed to iterate dialogue directory '%s': %s", dialogPath.string().c_str(), errorCode.message().c_str());
            errorCode.clear();
            continue;
        }

        const auto& entry = *it;
        if (!entry.is_regular_file(errorCode) || errorCode)
        {
            errorCode.clear();
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

bool DialogueManager::HasActiveDialogue() const
{
    return m_dialogueState.HasDialogue();
}

void DialogueManager::SetDialogue(const std::string &key)
{
    const Dialogue *dialogue = GetDialogue(key);
    if (!dialogue)
    {
        return;
    }

    m_dialogueState.SetDialogue(dialogue, key);
}

void DialogueManager::ClearDialogue()
{
    m_dialogueState.Clear();
}

void DialogueManager::Update(float deltaTime)
{
    m_dialogueState.Update(deltaTime);
}

bool DialogueManager::AdvanceDialogue()
{
    return m_dialogueState.Advance();
}

const DialogueRuntimeState &DialogueManager::GetRuntimeState() const
{
    return m_dialogueState;
}

DialogueRuntimeState &DialogueManager::GetRuntimeState()
{
    return m_dialogueState;
}
