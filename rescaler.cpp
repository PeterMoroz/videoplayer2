#include "rescaler.h"
#include "picture_writer.h"

extern "C"
{
#include <libavutil/frame.h>
#include <libswscale/swscale.h>
}

#include <iostream>


Rescaler::Rescaler()
	: _context(NULL)
	, _pictureWriter(NULL)
{
	for (size_t i = 0; i < 4; i++)
	{
		_pictureData[i] = NULL;
	}
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

void Rescaler::setPictureBuffer(uint8_t* pictureData[4], int pictureLinesize[4])
{
	for (size_t i = 0; i < 4; i++)
	{
		_pictureData[i] = pictureData[i];
		_pictureLinesize[i] = pictureLinesize[i];
	}
}

void Rescaler::setPictureWriter(PictureWriter* writer)
{
	_pictureWriter = writer;
}

void Rescaler::acceptFrame(const AVFrame* frame)
{
	if (!frame)
	{
		return;
	}

	if (!_context)
	{
		throw std::logic_error("Scale context is not initialized.");
	}

	if (_pictureData[0] == NULL || _pictureLinesize[0] == 0)
	{
		throw std::logic_error("Picture buffer is not initialized.");
	}

	int height = sws_scale(_context, reinterpret_cast<uint8_t const* const*>(frame->data), frame->linesize, 0, frame->height, _pictureData, _pictureLinesize);

	if (_pictureWriter)
	{
		_pictureWriter->savePicture(_pictureData, _pictureLinesize, _dstWidth, _dstHeight);
	}
}
