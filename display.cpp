#include "display.h"
#include "image_buffer.h"

#include <SDL.h>

#include <iostream>

Display::Display()
	: _window(NULL)
	, _renderer(NULL)
	, _texture(NULL)
{

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
}

int Display::addWindow(const char* title, int x, int y, int w, int h)
{
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

	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, w, h);
	if (!texture)
	{
		std::cerr << "SDL_CreateTexture() failed. error: " << SDL_GetError() << std::endl;
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		return -1;
	}

	_window = window;
	_renderer = renderer;
	_texture = texture;

	return 0;
}

void Display::updateWindow(int wndIndex)
{
	int ret = SDL_UpdateYUVTexture(_texture, NULL,
		_pictureData[0], _pictureLinesize[0],
		_pictureData[1], _pictureLinesize[1],
		_pictureData[2], _pictureLinesize[2]);

	if (ret < 0)
	{
		std::cerr << "SDL_UpdateYUVTexture() failed. error: "
			<< SDL_GetError() << " ( " << ret << " ) " << std::endl;
	}
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
