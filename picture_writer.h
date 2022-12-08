#pragma once

#include <cstdint>

class PictureWriter final
{
public:
	PictureWriter() = default;
	~PictureWriter() = default;

	void savePicture(uint8_t* data[4], int linesize[4], int width, int height);

private:
	unsigned _pictureCount = 0;
	unsigned _maxPictureCount = 1000;
};
