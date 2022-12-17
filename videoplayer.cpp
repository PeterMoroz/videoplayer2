#include "videoplayer.h"

#include "audio_output_device.h"
#include "video_output_device.h"

#include "decoder.h"

extern "C"
{
#include <libavcodec/avcodec.h>
}

#include <chrono>
#include <iostream>
#include <thread>

Videoplayer::Videoplayer(VideoOutputDevice& videoOutputDevice, AudioOutputDevice& audioOutputDevice)
	: _videoOutputDevice(videoOutputDevice)
	, _audioOutputDevice(audioOutputDevice)
{

}

Videoplayer::~Videoplayer()
{
	releasePictureBuffer();
}

bool Videoplayer::Open(const char* url)
{
	if (!_demuxer.Open(url))
	{
		std::cerr << "Couuld not open " << url << std::endl;
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
	const int dstFrameFormat = AV_PIX_FMT_YUV420P;// AV_PIX_FMT_RGB24;

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

	_rescaler.setOutputBuffer(&_pictureBuffer);

	_videoOutputDevice.setImageBuffer(&_pictureBuffer);

	videostreamDecoder->setFrameReceiver([this](AVFrame* frame){
		_rescaler.scaleFrame(frame);
		_videoOutputDevice.updateWindow(0);
		_videoOutputDevice.presentWindow(0);
		std::this_thread::sleep_for(std::chrono::milliseconds(40)); // 25 FPS
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