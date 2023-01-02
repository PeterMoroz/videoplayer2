#pragma once

#include <cstdint>
#include <cstddef>

struct SwrContext;

struct AVFrame;

class Resampler final
{
public:
	Resampler(const Resampler&) = delete;
	Resampler& operator=(const Resampler&) = delete;

	Resampler();
	~Resampler();

	bool init(int inChannels, int outChannels,
		int inSampleFormat, int outSampleFormat,
		int inChannelLayout, int outChannelLayout,
		int inSampleRate, int outSampleRate);

	int resampleFrame(const AVFrame* frame);

	void setOutputBuffer(uint8_t* outBuffer, int outBufferSize);

private:
	SwrContext* _context;
	int _inSampleRate = 0;
	int _outSampleRate = 0;
	int _outChannels = 0;
	int _outSampleFormat = 0;
	
	uint8_t* _outBuffer = NULL;
	int _outBufferSize = 0;
};
