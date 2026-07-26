#include "InputSystem.h"

void InputSystem::BeginFrame()
{
	m_keysPressed.fill(false);
	m_keysReleased.fill(false);

	m_mouseButtonsPressed.fill(false);
	m_mouseButtonsReleased.fill(false);
}

void InputSystem::ProcessEvent(const SDL_Event& event)
{
	switch (event.type)
	{
	case SDL_KEYDOWN:
	{
		if (event.key.repeat != 0)
		{
			break;
		}

		const SDL_Scancode scancode = event.key.keysym.scancode;
		if (scancode < 0 || scancode >= SDL_NUM_SCANCODES)
		{
			break;
		}

		if (!m_keysDown[scancode])
		{
			m_keysPressed[scancode] = true;
		}

		m_keysDown[scancode] = true;
		break;
	}

	case SDL_KEYUP:
	{
		const SDL_Scancode scancode = event.key.keysym.scancode;
		if (scancode < 0 || scancode >= SDL_NUM_SCANCODES)
		{
			break;
		}

		if (m_keysDown[scancode])
		{
			m_keysReleased[scancode] = true;
		}

		m_keysDown[scancode] = false;
		break;
	}

	case SDL_MOUSEMOTION:
		m_mouseX = event.motion.x;
		m_mouseY = event.motion.y;
		break;

	case SDL_MOUSEBUTTONDOWN:
	{
		m_mouseX = event.button.x;
		m_mouseY = event.button.y;

		const std::size_t button = static_cast<std::size_t>(event.button.button);
		if (button >= m_mouseButtonsDown.size())
		{
			break;
		}

		if (!m_mouseButtonsDown[button])
		{
			m_mouseButtonsPressed[button] = true;
		}

		m_mouseButtonsDown[button] = true;
		break;
	}

	case SDL_MOUSEBUTTONUP:
	{
		m_mouseX = event.button.x;
		m_mouseY = event.button.y;

		const std::size_t button = static_cast<std::size_t>(event.button.button);
		if (button >= m_mouseButtonsDown.size())
		{
			break;
		}

		if (m_mouseButtonsDown[button])
		{
			m_mouseButtonsReleased[button] = true;
		}

		m_mouseButtonsDown[button] = false;
		break;
	}

	case SDL_WINDOWEVENT:
		if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
		{
			Reset();
		}
		break;

	default:
		break;
	}
}

void InputSystem::Reset()
{
	m_keysDown.fill(false);
	m_keysPressed.fill(false);
	m_keysReleased.fill(false);

	m_mouseButtonsDown.fill(false);
	m_mouseButtonsPressed.fill(false);
	m_mouseButtonsReleased.fill(false);
}

bool InputSystem::IsKeyDown(SDL_Scancode scancode) const
{
	if (scancode < 0 || scancode >= SDL_NUM_SCANCODES)
	{
		return false;
	}

	return m_keysDown[scancode];
}

bool InputSystem::WasKeyPressed(SDL_Scancode scancode) const
{
	if (scancode < 0 || scancode >= SDL_NUM_SCANCODES)
	{
		return false;
	}

	return m_keysPressed[scancode];
}

bool InputSystem::WasKeyReleased(SDL_Scancode scancode) const
{
	if (scancode < 0 || scancode >= SDL_NUM_SCANCODES)
	{
		return false;
	}

	return m_keysReleased[scancode];
}

bool InputSystem::IsMouseButtonDown(Uint8 button) const
{
	const std::size_t index = static_cast<std::size_t>(button);
	if (index >= m_mouseButtonsDown.size())
	{
		return false;
	}

	return m_mouseButtonsDown[index];
}

bool InputSystem::WasMouseButtonPressed(Uint8 button) const
{
	const std::size_t index = static_cast<std::size_t>(button);
	if (index >= m_mouseButtonsPressed.size())
	{
		return false;
	}

	return m_mouseButtonsPressed[index];
}

bool InputSystem::WasMouseButtonReleased(Uint8 button) const
{
	const std::size_t index = static_cast<std::size_t>(button);
	if (index >= m_mouseButtonsReleased.size())
	{
		return false;
	}

	return m_mouseButtonsReleased[index];
}