#pragma once

#include "video_output_device.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;


class Display final : public VideoOutputDevice
{
public:
	Display();
	~Display() override;

	int addWindow(const char* title, int x, int y, int w, int h, PixelFormat pixelFormat) override;
	void updateWindow(int wndIndex) override;
	bool presentWindow(int wndIndex) override;

private:
	SDL_Window* _window;
	SDL_Renderer* _renderer;
	SDL_Texture* _texture;
};
