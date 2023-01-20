#pragma once

class Clock
{
public:
	Clock(const Clock&) = delete;
	Clock& operator=(const Clock&) = delete;

	Clock() = default;
	~Clock() = default;

	double value() const { return _value; }
	void setTimebase(const double& timebase) { _timebase = timebase; }

protected:
	double _value = 0.0;
	double _timebase = 0.0;
};
