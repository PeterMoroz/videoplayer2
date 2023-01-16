#include "image_buffer.h"

extern "C"
{
#include <libavutil/imgutils.h>
}

#include <SDL.h>
#include <iostream>

ImageBuffer::Lock::Lock(SDL_mutex* mutex)
	: _mutex(mutex)
{
	if (SDL_LockMutex(_mutex) < 0)
	{
		std::cerr << "SDL_LockMutex() failed. error: " << SDL_GetError() << std::endl;
	}
	else
	{
		_locked = true;
	}
}

ImageBuffer::Lock::~Lock()
{
	if (_locked)
	{
		if (SDL_UnlockMutex(_mutex) < 0)
		{
			std::cerr << "SDL_UnlockMutex() failed. error: " << SDL_GetError() << std::endl;
		}
	}
}


ImageBuffer::ImageBuffer()
	: _mutex(NULL, &SDL_DestroyMutex)
	, _cond(NULL, &SDL_DestroyCond)
{
	std::unique_ptr<SDL_mutex, MutexDeleter> mutex(SDL_CreateMutex(), &SDL_DestroyMutex);
	if (!mutex)
	{
		std::cerr << "SDL_CreateMutex() failed. error: " << SDL_GetError() << std::endl;
		throw std::runtime_error("Couldn't create SDL mutex.");
	}

	std::unique_ptr<SDL_cond, CondDeleter> cond(SDL_CreateCond(), &SDL_DestroyCond);
	if (!cond)
	{
		std::cerr << "SDL_CreateCond() failed. error: " << SDL_GetError() << std::endl;
		throw std::runtime_error("Couldn't create SDL cond.");
	}

	_mutex.swap(mutex);
	_cond.swap(cond);
}

ImageBuffer::~ImageBuffer()
{
	release();
}

bool ImageBuffer::allocate(int width, int height, int format, int align/* = 1*/)
{
	release();
	int ret = av_image_alloc(_image_data, _linesize, width, height, static_cast<AVPixelFormat>(format), align);
	if (ret < 0)
	{
		std::cerr << "av_image_alloc() failed. ret = " << ret << std::endl;
		return false;
	}
	return true;
}

void ImageBuffer::release()
{
	av_freep(&_image_data[0]);
	for (size_t i = 0; i < 4; i++)
	{
		_image_data[i] = NULL;
		_linesize[i] = 0;
	}
}

bool ImageBuffer::acquireWrite(uint8_t* image_data[], int linesize[])
{
	Lock lock(_mutex.get());
	if (!lock.locked())
	{
		return false;
	}

	while (!_empty)
	{
		_isWaiting = true;
		if (SDL_CondWait(_cond.get(), _mutex.get()) == -1)
		{
			std::cerr << "SDL_CondWait() failed. error: " << SDL_GetError() << std::endl;
			return false;
		}
		_isWaiting = false;
	}
	
	for (size_t i = 0; i < 4; i++)
	{
		image_data[i] = _image_data[i];
		linesize[i] = _linesize[i];
	}

	// TO DO: it is needed a "lock" variable which will leave no opportunity to race conditions, like 
	// "read untill write is not finished", "write to buffer which is currently been writtening".

	return true;
}

bool ImageBuffer::releaseWrite()
{
	Lock lock(_mutex.get());
	if (!lock.locked())
	{
		return false;
	}
	_empty = false;
	return true;
}

void ImageBuffer::acquireRead(uint8_t* image_data[], int linesize[])
{
	for (size_t i = 0; i < 4; i++)
	{
		image_data[i] = _image_data[i];
		linesize[i] = _linesize[i];
	}
}

bool ImageBuffer::releaseRead()
{
	Lock lock(_mutex.get());
	if (!lock.locked())
	{
		return false;
	}
	_empty = true;
	SDL_CondSignal(_cond.get());
	return true;
}

void ImageBuffer::setPTS(const double& pts)
{
	_pts = pts;
}

double ImageBuffer::getPTS() const
{
	return _pts;
}
