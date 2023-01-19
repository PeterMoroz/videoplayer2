#include "audio_clock.h"

extern "C"
{
#include <libavcodec\packet.h>
}


void AudioClock::update(AVPacket* packet)
{
	setValue(_timebase * packet->pts);
}

void AudioClock::update(unsigned nbytes)
{
	increment(static_cast<double>(nbytes) / static_cast<double>(_bytesPerSecond));
}

void AudioClock::setBytesPerSecond(int sampleRate, int numOfChannels, int bytesPerSample)
{
	_bytesPerSecond = sampleRate * numOfChannels * bytesPerSample;
}