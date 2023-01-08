#include "time.h"
#include "sdl_library.h"

#include <iostream>

#include <SDL.h>

Time& Time::getInstance()
{
	static Time instance;
	return instance;
}

Time::~Time()
{
	SDL_Library::getInstance().quitSubsystem(SDL_INIT_TIMER);
}

Time::Time()
{
	if (!SDL_Library::getInstance().initSubsystem(SDL_INIT_TIMER))
	{
		std::cerr << "SDL init timer failed." << std::endl;
		throw std::runtime_error("Can not initialize timer subsystem (SDL).");
	}
}

void Time::delay(std::uint32_t ms)
{
	SDL_Delay(ms);
}

bool Time::addTimer(TimerCallback cb, std::uint32_t delay, void* userdata)
{
	SDL_TimerID id = SDL_AddTimer(delay, cb, userdata);
	if (id == 0)
	{
		std::cerr << "SDL_AddTimer() failed. error: " << SDL_GetError() << std::endl;
		return false;
	}
	return true;
}
