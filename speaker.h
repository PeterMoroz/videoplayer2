#pragma once

#include "audio_output_device.h"

#include <cstdint>

class Speaker final : public AudioOutputDevice
{
public:
	Speaker();
	~Speaker() override;

	bool init(int sampleRate, int channelsNum, int samplesNum, SampleFormat sampleFormat) override;
	void start() override;

private:

	static void audioCallback(void* userdata, uint8_t* stream, int length);
	void handleAudioCallback(uint8_t* stream, int length);

	uint32_t _audioDeviceId = 0;

	static const size_t AUDIO_BUFFER_SIZE = 65536;
	uint8_t _audioBuffer[AUDIO_BUFFER_SIZE];

	uint32_t _audioDataLength = 0;
	uint32_t _audioBuffeIndex = 0;

	int _sampleFormatNative = 0;
};
