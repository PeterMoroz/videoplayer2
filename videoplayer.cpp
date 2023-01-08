#include "videoplayer.h"

#include "audio_output_device.h"
#include "video_output_device.h"

#include "event_dispatcher.h"
#include "sdl_library.h"

#include "decoder.h"
#include "time.h"

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


int getSampleFormat(AudioOutputDevice::SampleFormat sampleFormat)
{
	switch (sampleFormat)
	{
	case AudioOutputDevice::SampleFormat_S16SYS:
		return AV_SAMPLE_FMT_S16;
	case AudioOutputDevice::SampleFormat_S32SYS:
		return AV_SAMPLE_FMT_S32;
	case AudioOutputDevice::SampleFormat_F32SYS:
		return AV_SAMPLE_FMT_FLT;
	default:
		return AV_SAMPLE_FMT_NONE;
	}
	return AV_SAMPLE_FMT_NONE;
}

int getChannelLayout(AudioOutputDevice::ChannelLayout channelLayout)
{
	switch (channelLayout)
	{
	case AudioOutputDevice::ChLayout_Mono:
		return AV_CH_LAYOUT_MONO;
	case AudioOutputDevice::ChLayout_Stereo:
		return AV_CH_LAYOUT_STEREO;
	default:
		return 0;
	}
	return 0;
}

}

Videoplayer::Videoplayer(VideoOutputDevice& videoOutputDevice, AudioOutputDevice& audioOutputDevice)
	: _videoOutputDevice(videoOutputDevice)
	, _audioOutputDevice(audioOutputDevice)
	, _demuxThread(std::bind(&Videoplayer::demuxRoutine, this))
	, _videoDecodeThread(std::bind(&Videoplayer::decodeVideoRoutine, this))
{
	using namespace std::placeholders;
	_audioOutputDevice.setQueryAudioDataCallback(std::bind(&Videoplayer::queryAudioSamples, this, _1, _2));

	EventDispatcher::getInstance().addHandler(EventDispatcher::Evt_Quit, std::bind(&Videoplayer::onQuitEvent, this));
	EventDispatcher::getInstance().addHandler(EventDispatcher::Evt_RefreshScreen, std::bind(&Videoplayer::onRefreshScreenEvent, this));
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

	// video stream
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

	using namespace std::placeholders;
	videostreamDecoder->setFrameReceiver(std::bind(&Videoplayer::onVideoFrame, this, _1));

	_videostreamIndex = videostreamIndex;
	_videostreamDecoder = std::move(videostreamDecoder);


	// audio stream
	int audiostreamIndex = _demuxer.getStreamIndex(Demuxer::StreamType::Audio);
	if (audiostreamIndex == -1)
	{
		std::cerr << "Could not get index of audiostream." << std::endl;
		return false;
	}

	std::unique_ptr<Decoder> audiostreamDecoder = _demuxer.createDecoder(audiostreamIndex);
	if (!audiostreamDecoder)
	{
		std::cerr << "Could not create audiostream decoder." << std::endl;
		return false;
	}

	int inChannels = 0;
	int inSampleFormat = 0;
	int inChannelLayout = 0;
	int inSampleRate = 0;

	const int outChannels = _audioOutputDevice.getNumberOfChannels();
	const int outSampleFormat = getSampleFormat(_audioOutputDevice.getSampleFormat());
	const int outChannelLayout = getChannelLayout(_audioOutputDevice.getChannelLayout());
	const int outSampleRate = _audioOutputDevice.getSampleRate();


	if (!audiostreamDecoder->getParameter("channels", inChannels))
	{
		std::cerr << "Could not get parameter (channels) of audiostream." << std::endl;
		return false;
	}

	if (!audiostreamDecoder->getParameter("sample-format", inSampleFormat))
	{
		std::cerr << "Could not get parameter (sample-format) of audiostream." << std::endl;
		return false;
	}

	if (!audiostreamDecoder->getParameter("channel-layout", inChannelLayout))
	{
		std::cerr << "Could not get parameter (channel-layout) of audiostream." << std::endl;
		return false;
	}

	if (!audiostreamDecoder->getParameter("sample-rate", inSampleRate))
	{
		std::cerr << "Could not get parameter (sample-rate) of audiostream." << std::endl;
		return false;
	}

	if (!_resampler.init(inChannels, outChannels, 
						inSampleFormat, outSampleFormat,
						inChannelLayout, outChannelLayout,
						inSampleRate, outSampleRate))
	{
		std::cerr << "Could not initialize resampler." << std::endl;
		return false;
	}

	audiostreamDecoder->setFrameReceiver(std::bind(&Videoplayer::onAudioFrame, this, _1));

	_audiostreamIndex = audiostreamIndex;
	_audiostreamDecoder = std::move(audiostreamDecoder);

	return true;
}

void Videoplayer::Play()
{
	if (!_demuxThread.create())
	{
		throw std::runtime_error("Could not create demuxer thread.");		
	}

	if (!_videoDecodeThread.create())
	{
		throw std::runtime_error("Could not create video decoder thread.");
	}

	_audioOutputDevice.start();
	scheduleScreenRefresh(40);

	EventDispatcher::getInstance().processEvents();

	_demuxThread.waitForFinished();
	_videoDecodeThread.waitForFinished();
}

void Videoplayer::Stop()
{

}

void Videoplayer::releasePictureBuffer()
{
	_pictureBuffer.release();
}

void Videoplayer::onAudioFrame(AVFrame* frame)
{
	_audioDataLength += _resampler.resampleFrame(frame);
}

void Videoplayer::onVideoFrame(AVFrame* frame)
{
	uint8_t* image_data[4] = { NULL };
	int linesize[4] = { 0 };

	if (!_pictureBuffer.acquireWrite(image_data, linesize))
	{
		std::cerr << "Could not acquired picture buffer to write." << std::endl;
		return;
	}

	_rescaler.setOutputBuffer(image_data, linesize);
	_rescaler.scaleFrame(frame);

	_pictureBuffer.releaseWrite();
}

int Videoplayer::queryAudioSamples(uint8_t* audioBuffer, int bufferSize)
{
	AVPacket packet;
	if (!_audioPacketQueue.dequeue(&packet))
	{
		std::cerr << "Couldn't dequeue audio-packet." << std::endl;
		return -1;
	}

	// TO DO: need to be refactored. pretty fuzzy. not decided yet how to make it more clear.
	_audioDataLength = 0;
	_resampler.setOutputBuffer(audioBuffer, bufferSize);

	if (!_audiostreamDecoder->sendPacket(&packet))
	{
		std::cerr << "Couldn't send packet to audio-decoder." << std::endl;
		av_packet_unref(&packet);
		return -1;
	}

	_resampler.setOutputBuffer(NULL, 0);
	av_packet_unref(&packet);
	return _audioDataLength;
}

int Videoplayer::demuxRoutine()
{
	AVPacket packet;
	av_init_packet(&packet);

	packet.data = NULL;
	packet.size = 0;

	bool failed = false;

	while (1)
	{
		if (_audioPacketQueue.size() > MAX_AUDIOQ_SIZE ||
			_videoPacketQueue.size() > MAX_VIDEOQ_SIZE)
		{
			Time::delay(10);
			continue;
		}

		if (!_demuxer.readFrame(&packet))
		{
			if (!_demuxer.isReadFailed()) 
			{	// no error. probably EOF, wait for user input
				Time::delay(100);
				continue;
			}
			else 
			{
				failed = true;
				break;
			}
		}

		if (packet.stream_index == _videostreamIndex)
		{
			if (!_videostreamDecoder->sendPacket(&packet))
			{
				std::cerr << "Couldn't send packet to video-decoder." << std::endl;
				av_packet_unref(&packet);
				failed = true;
				break;
			}
		}
		else if (packet.stream_index == _audiostreamIndex)
		{
			if (!_audioPacketQueue.enqueue(&packet))
			{
				std::cerr << "Couldn't enqueue packet to audio-queue." << std::endl;
				av_packet_unref(&packet);
				failed = true;
				break;
			}
		}
		else
		{
			av_packet_unref(&packet);
		}
	}

	if (!failed)
	{
		// TO DO: check of state variables should be refactored. active wait is bad idea
		while (!_quit)
		{
			Time::delay(100);
		}
	}

	// TO DO: push quit event

	return 0;
}

int Videoplayer::decodeVideoRoutine()
{
	AVPacket packet;
	bool failed = false;
	while (1)
	{
		if (!_videoPacketQueue.dequeue(&packet))
		{
			std::cerr << "Couldn't dequeue video-packet." << std::endl;
			failed = true;
			break;
		}

		if (!_videostreamDecoder->sendPacket(&packet))
		{
			std::cerr << "Couldn't send packet to video-decoder." << std::endl;
			av_packet_unref(&packet);
			failed = true;
			break;
		}

		av_packet_unref(&packet);
	}
	return 0;
}

void Videoplayer::scheduleScreenRefresh(std::uint32_t delay)
{
	if (!Time::getInstance().addTimer(&Videoplayer::onScheduleScreenRefresh, delay, this))
	{
		std::cerr << "Could not create timer to screen refresh. " << std::endl;
	}
}

std::uint32_t Videoplayer::onScheduleScreenRefresh(std::uint32_t, void* userdata)
{
	if (!EventDispatcher::getInstance().pushEvent(EventDispatcher::Evt_RefreshScreen, userdata))
	{
		std::cerr << "Could not schedule refresh screen event." << std::endl;
	}
	return 0;
}

void Videoplayer::onQuitEvent()
{
	_quit = true;
}

void Videoplayer::onRefreshScreenEvent()
{
	if (_pictureBuffer.empty()) 
	{
		scheduleScreenRefresh(40);
	}
	else
	{
		scheduleScreenRefresh(80);

		uint8_t* image_data[4] = { NULL };
		int linesize[4] = { 0 };

		_pictureBuffer.acquireRead(image_data, linesize);
		_videoOutputDevice.updateWindow(0, image_data, linesize);
		if (!_pictureBuffer.releaseRead())
		{
			std::cerr << "An error when release read operation on picture buffer." << std::endl;
		}
		_videoOutputDevice.presentWindow(0);		
	}
}
