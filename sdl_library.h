#pragma once

#include <cstdint>

class SDL_Library final
{
public:
	~SDL_Library();
	static SDL_Library& getInstance();

private:
	SDL_Library();

	SDL_Library(const SDL_Library&) = delete;
	SDL_Library& operator=(const SDL_Library&) = delete;

public:
	bool initSubsystem(uint32_t id);
	void quitSubsystem(uint32_t id);

	void delay(uint32_t ms);
};
