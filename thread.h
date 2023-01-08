#pragma once

#include <cstddef>
#include <cstdint>

#include <functional>

// A wrapper for SDL_Thread

struct SDL_Thread;

class Thread final
{
public:
	Thread(const Thread&) = delete;
	Thread& operator=(const Thread&) = delete;

	explicit Thread(std::function<int()>&& work, const char* name="SDL_Thread");
	~Thread();

	bool create();
	int waitForFinished();

private:
	std::function<int()> _work;
	const char* _name = "";
	SDL_Thread* _thread = NULL;

	static int workDriver(void*);
};
