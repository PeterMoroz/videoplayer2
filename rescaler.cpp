#include "rescaler.h"
#include "image_buffer.h"

extern "C"
{
#include <libavutil/frame.h>
#include <libswscale/swscale.h>
}

#include <iostream>


Rescaler::Rescaler()
	: _context(NULL)
{

}

Rescaler::~Rescaler()
{
	if (_context)
	{
		sws_freeContext(_context);
		_context = NULL;
	}
}

bool Rescaler::init(int srcWidth, int srcHeight, int srcFormat, int dstWidth, int dstHeight, int dstFormat, int flags)
{
	_context = sws_getContext(srcWidth, srcHeight, static_cast<AVPixelFormat>(srcFormat),
							dstWidth, dstHeight, static_cast<AVPixelFormat>(dstFormat), 
							flags, NULL, NULL, NULL);
	if (!_context)
	{
		std::cerr << "sws_getContext() failed." << std::endl;
		return false;
	}

	_dstWidth = dstWidth;
	_dstHeight = dstHeight;

	return true;
}

void Rescaler::scaleFrame(const AVFrame* frame)
{
	if (!frame)
	{
		return;
	}
	
	if (!_context)
	{
		throw std::logic_error("Scale context is not initialized.");
	}
	
	if (_image_data[0] == NULL || _linesize[0] == 0)
	{
		throw std::logic_error("Picture buffer is not initialized.");
	}
	
	int height = sws_scale(_context, reinterpret_cast<uint8_t const* const*>(frame->data), frame->linesize, 0, frame->height, _image_data, _linesize);
}
