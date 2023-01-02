#pragma once

#include "demuxer.h"
#include "rescaler.h"
#include "resampler.h"
#include "image_buffer.h"
#include "packet_queue.h"

#include <memory>

class AudioOutputDevice;
class VideoOutputDevice;

class Decoder;

struct AVFrame;

class Videoplayer final
{
public:
	Videoplayer(VideoOutputDevice& videoOutputDevice, AudioOutputDevice& audioOutputDevice);
	~Videoplayer();

	Videoplayer(const Videoplayer&) = delete;
	Videoplayer& operator=(const Videoplayer&) = delete;

	bool Open(const char* url);
	void Play();
	void Stop();

private:
	void releasePictureBuffer();

	void onAudioFrame(AVFrame* frame);
	void onVideoFrame(AVFrame* frame);

	int queryAudioSamples(uint8_t* audioBuffer, int bufferSize);

private:
	VideoOutputDevice& _videoOutputDevice;
	AudioOutputDevice& _audioOutputDevice;

	Demuxer _demuxer;
	int _videostreamIndex = -1;
	int _audiostreamIndex = -1;
	std::unique_ptr<Decoder> _videostreamDecoder;
	std::unique_ptr<Decoder> _audiostreamDecoder;

	Rescaler _rescaler;
	Resampler _resampler;

	ImageBuffer _pictureBuffer;
	bool _quit = false;
	PacketQueue _audioPacketQueue;
	int _audioDataLength = 0;
};
