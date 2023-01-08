#pragma once

#include "demuxer.h"
#include "rescaler.h"
#include "resampler.h"
#include "image_buffer.h"
#include "packet_queue.h"
#include "thread.h"

#include <memory>

class AudioOutputDevice;
class VideoOutputDevice;

class Decoder;

struct AVFrame;

class Videoplayer final
{
	static const int MAX_AUDIOQ_SIZE = 5 * 16 * 1024;
	static const int MAX_VIDEOQ_SIZE = 5 * 256 * 1024;

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

	int demuxRoutine();
	int decodeVideoRoutine();

	void scheduleScreenRefresh(std::uint32_t delay);
	static std::uint32_t onScheduleScreenRefresh(std::uint32_t, void* userdata);


	// events' handlers
	void onQuitEvent();
	void onRefreshScreenEvent();

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
	PacketQueue _videoPacketQueue;
	int _audioDataLength = 0;

	Thread _demuxThread;
	Thread _videoDecodeThread;
};
