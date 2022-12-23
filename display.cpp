#include "display.h"
#include "image_buffer.h"
#include "sdl_library.h"

#include <SDL.h>

#include <iostream>

Display::Display()
	: _window(NULL)
	, _renderer(NULL)
	, _texture(NULL)
{
	if (!SDL_Library::getInstance().initSubsystem(SDL_INIT_VIDEO))
	{
		std::cerr << "SDL init video failed." << std::endl;
		throw std::runtime_error("Can not initialize video subsystem (SDL).");
	}
}

Display::~Display()
{
	if (_texture)
	{
		SDL_DestroyTexture(_texture);
		_texture = NULL;
	}

	if (_renderer)
	{
		SDL_DestroyRenderer(_renderer);
		_renderer = NULL;
	}

	if (_window)
	{		
		SDL_DestroyWindow(_window);
		_window = NULL;
	}

	SDL_Library::getInstance().quitSubsystem(SDL_INIT_VIDEO);
}

int Display::addWindow(const char* title, int x, int y, int w, int h, PixelFormat pixelFormat)
{
	uint32_t pixFormat = SDL_PIXELFORMAT_UNKNOWN;
	switch (pixelFormat)
	{
	case PixFormat_RGB24:
		pixFormat = SDL_PIXELFORMAT_RGB24;
		break;
	default:
		std::cerr << "Unsupported pixel format." << std::endl;
		return -1;
	}

	SDL_Window* window = SDL_CreateWindow(title, x, y, w, h, SDL_WINDOW_SHOWN);
	if (!window)
	{
		std::cerr << "SDL_CreateWindow() failed. error: " << SDL_GetError() << std::endl;
		return -1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer)
	{
		std::cerr << "SDL_CreateRenderer() failed. error: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(window);
		return -1;
	}

	SDL_Texture* texture = SDL_CreateTexture(renderer, pixFormat, SDL_TEXTUREACCESS_STREAMING, w, h);
	if (!texture)
	{
		std::cerr << "SDL_CreateTexture() failed. error: " << SDL_GetError() << std::endl;
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		return -1;
	}

	_width = w;
	_height = h;
	_pixelFormat = pixelFormat;

	_window = window;
	_renderer = renderer;
	_texture = texture;

	return 0;
}

void Display::updateWindow(int wndIndex)
{
	void* pixels[1] = { NULL };
	int pitch[1] = { 0 };

	int ret = 0;
	ret = SDL_LockTexture(_texture, NULL, (void **)pixels, pitch);
	if (ret < 0)
	{
		std::cerr << "SDL_LockTexture() failed. error: "
			<< SDL_GetError() << " ( " << ret << " ) " << std::endl;
		return;
	}

	std::memcpy(pixels[0], _image_data[0], pitch[0] * _height);

	SDL_UnlockTexture(_texture);
}

bool Display::presentWindow(int wndIndex)
{
	int ret = 0;
	ret = SDL_RenderClear(_renderer);
	if (ret != 0)
	{
		std::cerr << "SDL_RenderClear() failed. error: " 
			<< SDL_GetError() << " ( " << ret << " ) " << std::endl;
		return false;
	}

	ret = SDL_RenderCopy(_renderer, _texture, NULL, NULL);
	if (ret != 0)
	{
		std::cerr << "SDL_RenderCopy() failed. error: " 
			<< SDL_GetError() << " ( " << ret << ") " << std::endl;
		return false;
	}

	SDL_RenderPresent(_renderer);
	return true;
}
