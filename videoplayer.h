#pragma once

#include "demuxer.h"
#include "rescaler.h"
#include "picture_writer.h"

#include <memory>

class AudioOutputDevice;
class VideoOutputDevice;

class Decoder;


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


private:
	VideoOutputDevice& _videoOutputDevice;
	AudioOutputDevice& _audioOutputDevice;

	Demuxer _demuxer;
	int _videostreamIndex = -1;
	std::unique_ptr<Decoder> _videostreamDecoder;

	Rescaler _rescaler;
	PictureWriter _pictureWriter;

	uint8_t* _pictureData[4] = { NULL };
	int _pictureLinesize[4] = { 0 };
};
