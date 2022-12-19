#pragma once

#include <cstdint>

class ImageBuffer;

class ImageBufferReader
{
protected:
	ImageBufferReader() = default;

public:
	virtual ~ImageBufferReader() = 0;

	ImageBufferReader(const ImageBufferReader&) = delete;
	ImageBufferReader& operator=(const ImageBufferReader&) = delete;

	void init(ImageBuffer& imageBuffer);

protected:
	uint8_t* _image_data[4] = { NULL };
	int _linesize[4] = { 0 };
};
