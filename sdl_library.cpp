#include "sdl_library.h"

#include <SDL.h>

#include <iostream>


SDL_Library::~SDL_Library()
{
	SDL_Quit();
}

SDL_Library& SDL_Library::getInstance()
{
	static SDL_Library instance;
	return instance;
}

SDL_Library::SDL_Library()
{

}

bool SDL_Library::initSubsystem(uint32_t id)
{
	int ret = SDL_InitSubSystem(id);
	if (ret < 0)
	{
		std::cerr << "SDL_InitSubsystem() failed. Error: " 
			<< SDL_GetError() << " ( " << ret << ") " << std::endl;
		return false;
	}
	return true;
}

void SDL_Library::quitSubsystem(uint32_t id)
{
	SDL_QuitSubSystem(id);
}
