#pragma once 
#include <SDL2/SDL.h>

struct CameraComponent
{
	CameraComponent() = default;
	CameraComponent(int x, int y, int width, int height)
		: m_viewport{ x, y, width, height }
	{
	}

	SDL_Rect m_viewport{ 0, 0, 800, 600 };

	bool m_shouldFollow{ true };
	int m_offsetX{ 0 };
	int m_offsetY{ 0 };
};