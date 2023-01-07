#include "image_buffer.h"

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

void ImageBuffer::getBuffer(uint8_t* image_data[], int linesize[])
{
	for (size_t i = 0; i < 4; i++)
	{
		image_data[i] = _image_data[i];
		linesize[i] = _linesize[i];
	}
}