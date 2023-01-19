#include "clock.h"

void Clock::setValue(const double& v)
{
	int value = _timescale * v;
	SDL_AtomicSet(&_value, value);
}

double Clock::getValue() const
{
	int value = SDL_AtomicGet(const_cast<SDL_atomic_t*>(&_value));
	return static_cast<double>(value) / static_cast<double>(_timescale);
}

void Clock::increment(const double& v)
{
	int delta = _timescale * v;
	SDL_AtomicAdd(&_value, delta);
}