#pragma once

class AudioOutputDevice final
{
public:
	AudioOutputDevice() = default;
	~AudioOutputDevice() = default;

	AudioOutputDevice(const AudioOutputDevice&) = delete;
	AudioOutputDevice& operator=(const AudioOutputDevice&) = delete;
};
