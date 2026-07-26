#pragma once
#include <SDL2/sdl.h>
#include <array>
#include <cstddef>

class InputSystem
{
public:
	/// <summary>
	/// Begins a new input frame. This should be called at the start of each frame to reset the state of keys and mouse buttons.
	/// </summary>
	void BeginFrame();

	/// <summary>
	/// Processes an SDL_Event and updates the input state accordingly. 
	/// This should be called for each event polled from the SDL event queue.
	/// </summary>
	/// <param name="event">The SDL_Event to process.</param>
	void ProcessEvent(const SDL_Event& event);

	/// <summary>
	/// Resets the input state, clearing all key and mouse button states. 
	/// This can be used to clear the input state when needed.
	/// </summary>
	void Reset(); 

	/// <summary>
	/// Checks if a specific key is currently being held down.
	/// </summary>
	/// <param name="scancode">The SDL_Scancode of the key to check.</param>
	/// <returns>True if the key is down, false otherwise.</returns>
	bool IsKeyDown(SDL_Scancode scancode) const;

	/// <summary>
	/// Checks if a specific key was pressed during the current frame.
	/// </summary>
	/// <param name="scancode">The SDL_Scancode of the key to check.</param>
	/// <returns>True if the key was pressed, false otherwise.</returns>
	bool WasKeyPressed(SDL_Scancode scancode) const;

	/// <summary>
	/// Checks if a specific key was released during the current frame.
	/// </summary>
	/// <param name="scancode">The SDL_Scancode of the key to check.</param>
	/// <returns>True if the key was released, false otherwise.</returns>
	bool WasKeyReleased(SDL_Scancode scancode) const;

	/// <summary>
	/// Checks if a specific mouse button is currently being held down.
	/// </summary>
	/// <param name="button">The SDL button to check.</param>
	/// <returns>True if the button is down, false otherwise.</returns>
	bool IsMouseButtonDown(Uint8 button) const;

	/// <summary>
	/// Checks if a specific mouse button was pressed during the current frame.
	/// </summary>
	/// <param name="button">The SDL button to check.</param>
	/// <returns>True if the button was pressed, false otherwise.</returns>
	bool WasMouseButtonPressed(Uint8 button) const;

	/// <summary>
	/// Checks if a specific mouse button was released during the current frame.
	/// </summary>
	/// <param name="button">The SDL button to check.</param>
	/// <returns>True if the button was released, false otherwise.</returns>
	bool WasMouseButtonReleased(Uint8 button) const;

	/// <summary>
	/// Gets the current X position of the mouse cursor.
	/// </summary>
	/// <returns>The X position of the mouse cursor.</returns>
	int GetMouseX() const { return m_mouseX; }

	/// <summary>
	/// Gets the current Y position of the mouse cursor.
	/// </summary>
	/// <returns>The Y position of the mouse cursor.</returns>
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