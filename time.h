#pragma once

#include <cstdint>

class Time final
{
public:
	using TimerCallback = std::uint32_t(*)(std::uint32_t, void*);
public:
	Time(const Time&) = delete;
	Time& operator=(const Time&) = delete;

	static Time& getInstance();

	static void delay(std::uint32_t ms);

	~Time();

	bool addTimer(TimerCallback cb, std::uint32_t delay, void* userdata);

private:
	Time();
};
