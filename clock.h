#pragma once

#include <SDL.h>

class Clock
{
public:
	Clock(const Clock&) = delete;
	Clock& operator=(const Clock&) = delete;

	Clock() = default;
	~Clock() = default;

	double value() const { return getValue(); }
	void setTimebase(const double& timebase) { _timebase = timebase; }

protected:
	void setValue(const double& v);
	double getValue() const;
	void increment(const double& v);

protected:
	// double _value = 0.0;
	double _timebase = 0.0;

private:
	SDL_atomic_t _value{ 0 };
	int _timescale = 1'000'000;	// keep microseconds, provide seconds-oriented interface /
};
