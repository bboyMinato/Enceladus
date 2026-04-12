#pragma once 
#include <string>
#include <SDL2/SDL.h>

struct WindowSettings
{
	int width{ 1480 };
	int height{ 900 };
	int posX{ SDL_WINDOWPOS_CENTERED };
	int posY{ SDL_WINDOWPOS_CENTERED };
	int flags{ SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE };
};

struct AudioSettings
{
	int musicVolume{ 34 };
	int soundVolume{ 34 };
};

struct AppSettings
{
	WindowSettings windowSettings;
	AudioSettings audioSettings;

	static AppSettings LoadFromFile(const std::string& filePath);
	bool SaveToFile(const std::string& filePath) const;
};
