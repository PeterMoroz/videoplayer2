#include "decoder.h"

extern "C"
{
#include <libavcodec/avcodec.h>
}

#include <cstring>
#include <iostream>


Decoder::~Decoder()
{
	if (_codec_context)
	{
		avcodec_free_context(&_codec_context);
		_codec_context = NULL;
	}

	if (_frame)
	{
		av_frame_free(&_frame);
		_frame = NULL;
	}
}

Decoder::Decoder(AVCodecContext* codec_context)
	: _codec_context(codec_context)
	, _frame(NULL)
{
	_frame = av_frame_alloc();
	if (!_frame)
	{
		throw std::runtime_error("av_frame_alloc() failed.");
	}
}

bool Decoder::getParameter(const char* key, int& value) const
{
	if (!_codec_context)
	{
		throw std::logic_error("Could't get decoder's parameter. Invalid codec context.");
	}

	switch (_codec_context->codec_type)
	{
	case AVMEDIA_TYPE_VIDEO:
	{
		if (!std::strcmp(key, "width"))
		{
			value = _codec_context->width;
			return true;
		}
		else if (!std::strcmp(key, "height"))
		{
			value = _codec_context->height;
			return true;
		}
		else if (!std::strcmp(key, "pixel-format"))
		{
			value = _codec_context->pix_fmt;
			return true;
		}

		std::cerr << "Unknown video decoder's parameter: " << key << std::endl;
		return false;
	}
	case AVMEDIA_TYPE_AUDIO:
	{
		if (!std::strcmp(key, "channels"))
		{
			value = _codec_context->channels;
			return true;
		}
		else if (!std::strcmp(key, "sample-format"))
		{
			value = _codec_context->sample_fmt;
			return true;
		}
		else if (!std::strcmp(key, "channel-layout"))
		{
			value = _codec_context->channel_layout;
			return true;
		}
		else if (!std::strcmp(key, "sample-rate"))
		{
			value = _codec_context->sample_rate;
			return true;
		}

		std::cerr << "Unknown audio decoder's parameter: " << key << std::endl;
		return false;
	}
	default:
		throw std::logic_error("Couldn't get decoder's parameter. Unsupported codec type.");
	}

	// unreachable
	return false;
}

bool Decoder::sendPacket(const AVPacket *packet)
{
	int ret = avcodec_send_packet(_codec_context, packet);
	if (ret < 0)
	{
		std::cerr << "avcodec_send_packet() failed. ret = " << ret << std::endl;
		return false;
	}

	while (ret >= 0)
	{
		ret = avcodec_receive_frame(_codec_context, _frame);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			break;
		else if (ret < 0)
		{
			std::cerr << "avcodec_receive_frame() failed. ret = " << ret << std::endl;
			return false;
		}

		if (_onFrameReady)
		{
			_onFrameReady(_frame);
		}
	}

	return true;
}

void Decoder::setFrameReceiver(std::function<void(AVFrame*)>&& onFrameReady)
{
	_onFrameReady = std::move(onFrameReady);
}
