#pragma once

#include "../ECS/DialogSegmentComponent.h"
#include "../ECS/Registry.h"

class DialogManager;
class RenderSystem;
class InputSystem;
class Registry;
enum class DialogSystemState : std::uint8_t;
enum class UpdateTextResult : std::uint8_t;

class DialogSystem {
  public:
    static void AdvanceDialog(Registry &registry, const DialogManager &dialogManager);
    static void InitDialog(const Entity &player, Registry &registry, const DialogManager &dialogManager);
    static void Update(const Entity &player, const InputSystem &input, Registry &registry, const DialogManager &dialogManager);
    static UpdateTextResult UpdateText(DialogSegmentComponent &segmentComponent, const DialogManager &dialogManager);

  private:
    static DialogSystemState m_state;
};

enum class DialogSystemState : std::uint8_t {
    Hide,
    Show
};

enum class UpdateTextResult : std::uint8_t {
    Advanced,
    Waiting,
    Exit
};