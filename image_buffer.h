#pragma once

#include <cstdint>

class ImageBuffer final
{
public:
	ImageBuffer(const ImageBuffer&) = delete;
	ImageBuffer& operator=(const ImageBuffer&) = delete;

	ImageBuffer(ImageBuffer&&);
	ImageBuffer& operator=(ImageBuffer&&);

	ImageBuffer() = default;
	~ImageBuffer();

	bool allocate(int width, int height, int format, int align = 1);
	void release();

	void provideAccess(uint8_t* data[], int linesize[]);

private:
	uint8_t* _image_data[4] = { NULL };
	int _linesize[4] = { 0 };
};
