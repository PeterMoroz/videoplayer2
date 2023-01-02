#pragma once

#include <cstdint>

class ImageBufferReader;
class ImageBufferWriter;

class ImageBuffer final
{
	friend class ImageBufferReader;
	friend class ImageBufferWriter;

public:
	ImageBuffer(const ImageBuffer&) = delete;
	ImageBuffer& operator=(const ImageBuffer&) = delete;

	ImageBuffer(ImageBuffer&&);
	ImageBuffer& operator=(ImageBuffer&&);

	ImageBuffer() = default;
	~ImageBuffer();

	void initReader(ImageBufferReader& reader);
	void initWriter(ImageBufferWriter& writer);

	bool allocate(int width, int height, int format, int align = 1);
	void release();

private:
	uint8_t* _image_data[4] = { NULL };
	int _linesize[4] = { 0 };
};
