#include "thread.h"

#include <cassert>
#include <iostream>

#include <SDL.h>

int Thread::workDriver(void* userdata)
{
	Thread* t = static_cast<Thread*>(userdata);
	assert(t != NULL && t->_work);
	return t->_work();
}

Thread::Thread(std::function<int()>&& work, const char* name /*= "SDL_Thread"*/)
	: _work(std::move(work))
	, _name(name)
{

}

Thread::~Thread()
{

}

bool Thread::create()
{
	_thread = SDL_CreateThread(&Thread::workDriver, _name, this);
	if (_thread == NULL)
	{
		std::cerr << "SDL_CreateThread() failed. error: " << SDL_GetError() << std::endl;
		return false;
	}
	return true;
}

int Thread::waitForFinished()
{
	if (_thread == NULL)
	{
		throw std::logic_error("Thread is not created.");
	}

	int status = 0;
	SDL_WaitThread(_thread, &status);
	_thread = NULL;
	return status;
}
