#pragma once

#include <cstddef>
#include <cstdint>

class ImageBuffer;

class VideoOutputDevice
{
public:
	VideoOutputDevice() = default;
	virtual ~VideoOutputDevice() = 0;

	VideoOutputDevice(const VideoOutputDevice&) = delete;
	VideoOutputDevice& operator=(const VideoOutputDevice&) = delete;

	virtual int addWindow(const char* title, int x, int y, int w, int h) = 0;
	virtual void updateWindow(int wndIndex) = 0;
	virtual bool presentWindow(int wndIndex) = 0;

	int getWidth() const { return _width; }
	int getHeight() const { return _height; }

	void setWidth(int width) { _width = width; }
	void setHeight(int height) { _height = height; }

	void setImageBuffer(ImageBuffer* imageBuffer);

protected:
	int _width = 0;
	int _height = 0;
	uint8_t* _pictureData[4] = { NULL };
	int _pictureLinesize[4] = { 0 };
};
