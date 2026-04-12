#define SDL_MAIN_HANDLED
#include "Core/Engine.h"

int main()
{
    SDL_SetMainReady();

	Engine engine;

	engine.Init();
	engine.Run();
	
    return 0;
}