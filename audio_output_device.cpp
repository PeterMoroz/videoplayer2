#include "audio_output_device.h"

AudioOutputDevice::~AudioOutputDevice()
{

}

void AudioOutputDevice::setQueryAudioDataCallback(QueryAudioDataCallback&& queryAudioData)
{
	_queryAudioData = std::move(queryAudioData);
}
