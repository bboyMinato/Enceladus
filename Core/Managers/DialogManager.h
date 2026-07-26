#pragma once
#include "../Utility/Dialogue.h"
#include "../Utility/DialogueRuntimeState.h"

#include <filesystem>
#include <map>
#include <string>

inline const std::filesystem::path dialogPath{"Assets/dialogs"};

class DialogueManager
{
  public:
    DialogueManager() = default;
    bool Init();
    const Dialogue *GetDialogue(const std::string &key) const;
    bool HasActiveDialogue() const;
    void SetDialogue(const std::string &key);
    void ClearDialogue();
    void Update(float deltaTime);
    bool AdvanceDialogue();
    const DialogueRuntimeState &GetRuntimeState() const;
    DialogueRuntimeState &GetRuntimeState();

  private:
    std::map<std::string, Dialogue> m_dialogs;
    DialogueRuntimeState m_dialogueState;
};
