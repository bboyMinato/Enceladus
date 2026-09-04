#pragma once
#include <SDL2/SDL.h>
#include <filesystem>

struct ControllerComponent
{
	bool isEnabled{ true };

	SDL_Scancode moveLeftPrimary{ };
	SDL_Scancode moveLeftSecondary{ };

	SDL_Scancode moveRightPrimary{ };
	SDL_Scancode moveRightSecondary{ };

	SDL_Scancode moveUpPrimary{ };
	SDL_Scancode moveUpSecondary{ };

	SDL_Scancode moveDownPrimary{ };
	SDL_Scancode moveDownSecondary{ };

	// Action keys
	SDL_Scancode interactPrimary{ };
	SDL_Scancode interactSecondary { };
	SDL_Scancode backKey{ };

	std::filesystem::path m_sourcePath{ };
};