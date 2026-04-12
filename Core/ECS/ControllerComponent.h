#pragma once
#include <SDL2/SDL.h>

struct ControllerComponent
{
	bool isEnabled{ true };

	SDL_Scancode moveLeftPrimary{ SDL_SCANCODE_A };
	SDL_Scancode moveLeftSecondary{ SDL_SCANCODE_LEFT };

	SDL_Scancode moveRightPrimary{ SDL_SCANCODE_D };
	SDL_Scancode moveRightSecondary{ SDL_SCANCODE_RIGHT };

	SDL_Scancode moveUpPrimary{ SDL_SCANCODE_W };
	SDL_Scancode moveUpSecondary{ SDL_SCANCODE_UP };

	SDL_Scancode moveDownPrimary{ SDL_SCANCODE_S };
	SDL_Scancode moveDownSecondary{ SDL_SCANCODE_DOWN };

	SDL_Scancode backKey{ SDL_SCANCODE_ESCAPE };
};