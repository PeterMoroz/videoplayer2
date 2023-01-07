#include "resampler.h"

#include <iostream>

extern "C"
{
#include <libavutil/frame.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
}


Resampler::Resampler()
	: _context(NULL)
{
	_context = swr_alloc();
	if (!_context)
	{
		std::cerr << "Could not allocate resample context." << std::endl;
		throw std::runtime_error("Could not allocate resample context.");
	}
}

Resampler::~Resampler()
{
	if (_context)
	{
		swr_free(&_context);
		_context = NULL;
	}
}

bool Resampler::init(int inChannels, int outChannels,
	int inSampleFormat, int outSampleFormat,
	int inChannelLayout, int outChannelLayout,
	int inSampleRate, int outSampleRate)
{
	if (!_context)
	{
		throw std::logic_error("Access to not allocated context.");
	}

	av_opt_set_int(_context, "in_channel_count", inChannels, 0);
	av_opt_set_int(_context, "out_channel_count", outChannels, 0);
	av_opt_set_int(_context, "in_sample_fmt", inSampleFormat, 0);
	av_opt_set_int(_context, "out_sample_fmt", outSampleFormat, 0);
	av_opt_set_int(_context, "in_channel_layout", inChannelLayout, 0);
	av_opt_set_int(_context, "out_channel_layout", outChannelLayout, 0);
	av_opt_set_int(_context, "in_sample_rate", inSampleRate, 0);
	av_opt_set_int(_context, "out_sample_rate", outSampleRate, 0);

	int ret = swr_init(_context);
	if (ret < 0)
	{
		std::cerr << "swr_init() failed. error: ret = " << ret << std::endl;
		return false;
	}

	_inSampleRate = inSampleRate;
	_outSampleRate = outSampleRate;

	_outChannels = outChannels;
	_outSampleFormat = outSampleFormat;

	return true;
}

int Resampler::resampleFrame(const AVFrame* frame)
{
	if (!frame)
	{
		return 0;
	}

	int64_t nsamplesDelay = swr_get_delay(_context, _inSampleRate);
	int nsamples = static_cast<int>(av_rescale_rnd(nsamplesDelay + frame->nb_samples, _inSampleRate, _outSampleRate, AV_ROUND_UP));

	uint8_t** samples = NULL;
	if (av_samples_alloc_array_and_samples(&samples, NULL, _outChannels, nsamples, static_cast<AVSampleFormat>(_outSampleFormat), 0) < 0)
	{
		std::cerr << "Could not allocate memory to store audiosamples!" << std::endl;
		return 0;
	}

	int nsamplesConverted = swr_convert(_context, samples, nsamples, (const uint8_t**)frame->data, frame->nb_samples);
	int nbytes = av_samples_get_buffer_size(NULL, _outChannels, nsamplesConverted, static_cast<AVSampleFormat>(_outSampleFormat), 1);

	std::memcpy(_outBuffer, samples[0], nbytes);

	bool copied = false;
	if (_outBufferSize - nbytes > 0)
	{
		_outBuffer += nbytes;
		_outBufferSize -= nbytes;
		copied = true;
	}

	if (samples)
	{
		av_freep(&samples[0]);
		av_freep(&samples);
	}

	return copied ? nbytes : 0;
}

void Resampler::setOutputBuffer(uint8_t* outBuffer, int outBufferSize)
{
	_outBuffer = outBuffer;
	_outBufferSize = outBufferSize;
}
