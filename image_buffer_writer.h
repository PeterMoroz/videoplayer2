#pragma once

#include <cstdint>

class ImageBuffer;

class ImageBufferWriter
{
protected:
	ImageBufferWriter() = default;

public:
	virtual ~ImageBufferWriter() = 0;

	void init(ImageBuffer& imageBuffer);

protected:
	uint8_t* _image_data[4] = { NULL };
	int _linesize[4] = { 0 };
};
