#pragma once

#include <cstddef>
#include <cstdint>

class VideoOutputDevice
{
public:
	enum PixelFormat
	{
		PixFormat_None = -1,
		PixFormat_RGB24 = 2,
	};

public:
	VideoOutputDevice() = default;
	virtual ~VideoOutputDevice() = 0;

	VideoOutputDevice(const VideoOutputDevice&) = delete;
	VideoOutputDevice& operator=(const VideoOutputDevice&) = delete;

	virtual int addWindow(const char* title, int x, int y, int w, int h, PixelFormat pixelFormat) = 0;
	virtual void updateWindow(int wndIndex, uint8_t* image_data[], int linesize[]) = 0;
	virtual bool presentWindow(int wndIndex) = 0;

	int getWidth() const { return _width; }
	int getHeight() const { return _height; }
	PixelFormat getPixelFormat() const { return _pixelFormat; }

protected:
	int _width = 0;
	int _height = 0;
	PixelFormat _pixelFormat = PixFormat_None;
};
