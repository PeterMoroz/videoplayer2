#include "image_buffer.h"
#include "image_buffer_reader.h"
#include "image_buffer_writer.h"

extern "C"
{
#include <libavutil/imgutils.h>
}

#include <iostream>


ImageBuffer::ImageBuffer(ImageBuffer&& obj)
{
	for (size_t i = 0; i < 4; i++)
	{
		_image_data[i] = obj._image_data[i];
		_linesize[i] = obj._linesize[i];

		obj._image_data[i] = NULL;
		obj._linesize[i] = 0;
	}
}

ImageBuffer& ImageBuffer::operator=(ImageBuffer&& obj)
{
	if (this != &obj)
	{
		for (size_t i = 0; i < 4; i++)
		{
			_image_data[i] = obj._image_data[i];
			_linesize[i] = obj._linesize[i];

			obj._image_data[i] = NULL;
			obj._linesize[i] = 0;
		}
	}
	return *this;
}

ImageBuffer::~ImageBuffer()
{
	release();
}

void ImageBuffer::initReader(ImageBufferReader& reader)
{
	reader.init(*this);
}

void ImageBuffer::initWriter(ImageBufferWriter& writer)
{
	writer.init(*this);
}

bool ImageBuffer::allocate(int width, int height, int format, int align/* = 1*/)
{
	release();
	int ret = av_image_alloc(_image_data, _linesize, width, height, static_cast<AVPixelFormat>(format), align);
	if (ret < 0)
	{
		std::cerr << "av_image_alloc() failed. ret = " << ret << std::endl;
		return false;
	}
	return true;
}

void ImageBuffer::release()
{
	av_freep(&_image_data[0]);
	for (size_t i = 0; i < 4; i++)
	{
		_image_data[i] = NULL;
		_linesize[i] = 0;
	}
}

//void ImageBuffer::provideAccess(uint8_t* data[], int linesize[])
//{
//	for (size_t i = 0; i < 4; i++)
//	{
//		data[i] = _image_data[i];
//		linesize[i] = _linesize[i];
//	}
//}
