#include "videoplayer.h"

#include "audio_output_device.h"
#include "video_output_device.h"

Videoplayer::Videoplayer(VideoOutputDevice& videoOutputDevice, AudioOutputDevice& audioOutputDevice)
	: _videoOutputDevice(videoOutputDevice)
	, _audioOutputDevice(audioOutputDevice)
{

}

Videoplayer::~Videoplayer()
{

}

void Videoplayer::Open(const char* uri)
{

}

void Videoplayer::Play()
{

}

void Videoplayer::Stop()
{

}
