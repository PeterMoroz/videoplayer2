#pragma once

#include <cstdint>
#include <functional>

class AudioOutputDevice
{
public:
	enum SampleFormat
	{
		SampleFormat_None = -1,
		SampleFormat_U16SYS = 1,
		SampleFormat_S16SYS = 2,
		SampleFormat_S32SYS = 3,
		SampleFormat_F32SYS = 4,
	};

	enum ChannelLayout
	{
		ChLayout_None = -1,
		ChLayout_Mono = 1,
		ChLayout_Stereo = 2,
	};

	using QueryAudioDataCallback = std::function<int(uint8_t*, int)>;

public:
	AudioOutputDevice() = default;
	virtual ~AudioOutputDevice() = 0;

	AudioOutputDevice(const AudioOutputDevice&) = delete;
	AudioOutputDevice& operator=(const AudioOutputDevice&) = delete;

	virtual bool init(int sampleRate, int channelsNum, int samplesNum, SampleFormat sampleFormat) = 0;
	virtual void start() = 0;

	int getNumberOfChannels() const { return _numOfChannels; }
	SampleFormat getSampleFormat() const { return _sampleFormat; }
	ChannelLayout getChannelLayout() const { return _channelLayout; }
	int getSampleRate() const { return _sampleRate; }

	void setQueryAudioDataCallback(QueryAudioDataCallback&& queryAudioData);

protected:
	int _numOfChannels = 0;
	SampleFormat _sampleFormat = SampleFormat_None;
	ChannelLayout _channelLayout = ChLayout_None;
	int _sampleRate = 0;

	QueryAudioDataCallback _queryAudioData;
};
