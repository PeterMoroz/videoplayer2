#pragma once

#include "clock.h"

struct AVPacket;

class AudioClock final : public Clock
{
public:

	void update(AVPacket* packet);
	void update(unsigned nbytes);
	void setBytesPerSecond(int sampleRate, int numOfChannels, int bytesPerSample);

private:
	int _bytesPerSecond = 0;
};
