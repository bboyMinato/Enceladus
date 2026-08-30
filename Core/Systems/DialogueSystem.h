#pragma once
#include <cstddef>
#include <string>

struct Dialogue;
struct DialogueRuntimeState;
class InputSystem;

class DialogueSystem final
{
public:
	static bool Start(DialogueRuntimeState& state, const Dialogue& dialogue, const std::string& startNode);
	static void Update(DialogueRuntimeState& state, const InputSystem& input);

private:
	static void Advance(DialogueRuntimeState& state);
	static void SelectOption(DialogueRuntimeState& state, std::size_t optionIndex);
	static void Close(DialogueRuntimeState& state);
};