#pragma once

#include <cstddef>
#include <cstdint>

#include "image_buffer_reader.h"

class VideoOutputDevice : public ImageBufferReader
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
	virtual void updateWindow(int wndIndex) = 0;
	virtual bool presentWindow(int wndIndex) = 0;

	int getWidth() const { return _width; }
	int getHeight() const { return _height; }
	PixelFormat getPixelFormat() const { return _pixelFormat; }

	//void setWidth(int width) { _width = width; }
	//void setHeight(int height) { _height = height; }


	// void setImageBuffer(ImageBuffer* imageBuffer);

protected:
	int _width = 0;
	int _height = 0;
	PixelFormat _pixelFormat = PixFormat_None;
	//uint8_t* _pictureData[4] = { NULL };
	//int _pictureLinesize[4] = { 0 };
};
