#pragma once
#include <filesystem>
#include <map>
#include <string>
#include <vector>

class DialogEntry;
class Dialog;

const auto dialogPath = std::filesystem::path("Assets/dialogs");

class DialogManager {
  public:
    DialogManager() = default;

    bool Init();
    const Dialog &GetDialog(const std::string &key) const;

  private:
    std::map<std::string, Dialog> m_dialogs;
};

class Dialog {
  public:
    Dialog(const std::filesystem::path &path);
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
