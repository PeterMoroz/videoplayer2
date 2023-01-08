#include "image_buffer.h"

extern "C"
{
#include <libavutil/imgutils.h>
}

#include <SDL.h>
#include <iostream>


ImageBuffer::ImageBuffer()
{
	_mutex = SDL_CreateMutex();
	if (!_mutex)
	{
		throw std::runtime_error("Could not create SDL mutex");
	}

	_cond = SDL_CreateCond();
	if (!_cond)
	{
		throw std::runtime_error("Could not create SDL cond");
	}
}

ImageBuffer::~ImageBuffer()
{
	release();

	SDL_DestroyMutex(_mutex);
	SDL_DestroyCond(_cond);
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
	SDL_LockMutex(_mutex);
	while (!_empty)
	{
		if (SDL_CondWait(_cond, _mutex) == -1)
		{
			std::cerr << "SDL_CondWait() failed. error: " << SDL_GetError() << std::endl;
			SDL_UnlockMutex(_mutex);
			return false;
		}
	}
	
	for (size_t i = 0; i < 4; i++)
	{
		image_data[i] = _image_data[i];
		linesize[i] = _linesize[i];
	}

	SDL_UnlockMutex(_mutex);
	return true;
}

void ImageBuffer::releaseWrite()
{
	SDL_LockMutex(_mutex);
	_empty = false;
	SDL_UnlockMutex(_mutex);
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
	SDL_LockMutex(_mutex);
	_empty = true;
	SDL_CondSignal(_cond);
	SDL_UnlockMutex(_mutex);
	return true;
}
