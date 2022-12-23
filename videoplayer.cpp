#include "videoplayer.h"

#include "audio_output_device.h"
#include "video_output_device.h"
#include "event_dispatcher.h"
#include "sdl_library.h"

#include "decoder.h"

extern "C"
{
#include <libavcodec/avcodec.h>
}

#include <iostream>

namespace
{

int getPixelFormat(VideoOutputDevice::PixelFormat pixFormat)
{
	switch (pixFormat)
	{
	case VideoOutputDevice::PixFormat_None:
		return AV_PIX_FMT_NONE;
	case VideoOutputDevice::PixFormat_RGB24:
		return AV_PIX_FMT_RGB24;
	default:
		return AV_PIX_FMT_NONE;
	}
	return AV_PIX_FMT_NONE;
}

}

Videoplayer::Videoplayer(VideoOutputDevice& videoOutputDevice, AudioOutputDevice& audioOutputDevice)
	: _videoOutputDevice(videoOutputDevice)
	, _audioOutputDevice(audioOutputDevice)
{
	auto onQuit = [this]() {
		_quit = true;
	};

	EventDispatcher::getInstance().addHandler(EventDispatcher::Evt_Quit, std::move(onQuit));
}

Videoplayer::~Videoplayer()
{
	releasePictureBuffer();
}

bool Videoplayer::Open(const char* url)
{
	if (!_demuxer.Open(url))
	{
		std::cerr << "Could not open " << url << std::endl;
		return false;
	}

	int videostreamIndex = _demuxer.getStreamIndex(Demuxer::StreamType::Video);
	if (videostreamIndex == -1)
	{
		std::cerr << "Could not get index of videostream." << std::endl;
		return false;
	}

	std::unique_ptr<Decoder> videostreamDecoder = _demuxer.createDecoder(videostreamIndex);
	if (!videostreamDecoder)
	{
		std::cerr << "Could not create videostream decoder." << std::endl;
		return false;
	}

	int srcFrameWidth = 0;
	int srcFrameHeight = 0;
	int srcFrameFormat = 0;

	const int dstFrameWidth = _videoOutputDevice.getWidth();
	const int dstFrameHeight = _videoOutputDevice.getHeight();
	const int dstFrameFormat = getPixelFormat(_videoOutputDevice.getPixelFormat());

	if (!videostreamDecoder->getParameter("width", srcFrameWidth))
	{
		std::cerr << "Could not get parameter (width) of videostream." << std::endl;
		return false;
	}

	if (!videostreamDecoder->getParameter("height", srcFrameHeight))
	{
		std::cerr << "Could not get parameter (height) of videostream." << std::endl;
		return false;
	}

	if (!videostreamDecoder->getParameter("pixel-format", srcFrameFormat))
	{
		std::cerr << "Could not get parameter (pixel-format) of videostream." << std::endl;
		return false;
	}

	if (!_rescaler.init(srcFrameWidth, srcFrameHeight, srcFrameFormat,
		dstFrameWidth, dstFrameHeight, dstFrameFormat,
		Rescaler::Bilinear | Rescaler::PrintInfo))
	{
		std::cerr << "Could not initialize rescaler." << std::endl;
		return false;
	}

	releasePictureBuffer();

	if (!_pictureBuffer.allocate(dstFrameWidth, dstFrameHeight, dstFrameFormat))
	{
		std::cerr << "Could not allocate picture buffer." << std::endl;
		return false;
	}

	_pictureBuffer.initWriter(_rescaler);
	_pictureBuffer.initReader(_videoOutputDevice);

	videostreamDecoder->setFrameReceiver([this](AVFrame* frame){
		_rescaler.scaleFrame(frame);
		_videoOutputDevice.updateWindow(0);
		_videoOutputDevice.presentWindow(0);
		SDL_Library::getInstance().delay(40); // 25 FPS
		EventDispatcher::getInstance().pollEvents();
	});

	_videostreamIndex = videostreamIndex;
	_videostreamDecoder = std::move(videostreamDecoder);

	return true;
}

void Videoplayer::Play()
{
	AVPacket packet;
	av_init_packet(&packet);

	packet.data = NULL;
	packet.size = 0;

	while (_demuxer.readFrame(&packet))
	{
		if (packet.stream_index == _videostreamIndex)
		{
			if (!_videostreamDecoder->sendPacket(&packet))
			{
				std::cerr << "Decoding of packet failed." << std::endl;
				break;
			}
		}
		av_packet_unref(&packet);
	}
}

void Videoplayer::Stop()
{

}

void Videoplayer::releasePictureBuffer()
{
	_pictureBuffer.release();
}