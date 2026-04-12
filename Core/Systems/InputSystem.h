#pragma once
#include <SDL2/sdl.h>
#include <array>
#include <cstddef>

class InputSystem
{
public:
	void BeginFrame();
	void ProcessEvent(const SDL_Event& event);
	void Reset(); 

	bool IsKeyDown(SDL_Scancode scancode) const;
	bool WasKeyPressed(SDL_Scancode scancode) const;
	bool WasKeyReleased(SDL_Scancode scancode) const;

	bool IsMouseButtonDown(Uint8 button) const;
	bool WasMouseButtonPressed(Uint8 button) const;
	bool WasMouseButtonReleased(Uint8 button) const;

	int GetMouseX() const { return m_mouseX; }
	int GetMouseY() const { return m_mouseY; }

private:
	static constexpr size_t s_mouseButtonCount = 8;

	std::array<bool, SDL_NUM_SCANCODES> m_keysDown{};
	std::array<bool, SDL_NUM_SCANCODES> m_keysPressed{};
	std::array<bool, SDL_NUM_SCANCODES> m_keysReleased{};

	std::array<bool, s_mouseButtonCount> m_mouseButtonsDown{};
	std::array<bool, s_mouseButtonCount> m_mouseButtonsPressed{};
	std::array<bool, s_mouseButtonCount> m_mouseButtonsReleased{};

	int m_mouseX{ 0 };
	int m_mouseY{ 0 };
};