#include "speaker.h"
#include "sdl_library.h"

#include <iostream>
#include <iomanip>

#include <SDL.h>


Speaker::Speaker()
{
	if (!SDL_Library::getInstance().initSubsystem(SDL_INIT_AUDIO))
	{
		std::cerr << "SDL init audio failed." << std::endl;
		throw std::runtime_error("Can not initialize audio subsystem (SDL).");
	}
}

Speaker::~Speaker()
{
	if (_audioDeviceId != 0)
	{
		SDL_CloseAudioDevice(_audioDeviceId);
	}

	SDL_Library::getInstance().quitSubsystem(SDL_INIT_AUDIO);
}

bool Speaker::init(int sampleRate, int channelsNum, int samplesNum, SampleFormat sampleFormat)
{
	SDL_AudioSpec desired, actual;
	SDL_memset(&desired, 0, sizeof(desired));
	SDL_memset(&actual, 0, sizeof(actual));

	SDL_AudioFormat audioFormat = 0;
	switch (sampleFormat)
	{
	case SampleFormat_U16SYS:
		audioFormat = AUDIO_U16SYS;
		break;
	case SampleFormat_S16SYS:
		audioFormat = AUDIO_S16SYS;
		break;
	case SampleFormat_S32SYS:
		audioFormat = AUDIO_S32SYS;
		break;
	case SampleFormat_F32SYS:
		audioFormat = AUDIO_F32SYS;
		break;
	default:
		std::cerr << "Unsupported sample format." << std::endl;
		return false;
	}

	desired.freq = sampleRate;
	desired.format = audioFormat;
	desired.channels = channelsNum;
	desired.silence = 0;
	desired.samples = samplesNum;
	desired.callback = audioCallback;
	desired.userdata = this;

	uint32_t audioDeviceId = SDL_OpenAudioDevice(NULL, 0, &desired, &actual, 0);	
	if (audioDeviceId == 0)
	{
		std::cerr << "SDL_OpenAudioDevice() failed. error: " << SDL_GetError() << std::endl;
		return false;
	}

	_numOfChannels = actual.channels;

	switch (actual.format)
	{
	case AUDIO_U16SYS:
		_sampleFormat = SampleFormat_U16SYS;
		break;
	case AUDIO_S16SYS:
		_sampleFormat = SampleFormat_S16SYS;
		break;
	case AUDIO_S32SYS:
		_sampleFormat = SampleFormat_S32SYS;
		break;
	case AUDIO_F32SYS:
		_sampleFormat = SampleFormat_F32SYS;
		break;
	default:
		std::cerr << "Sample format returned by audio-device ("
			<< std::hex << std::setw(4) << std::setfill('0') << actual.format 
			<< ") is not supported yet." << std::endl;
		return false;
	}

	_sampleFormatNative = actual.format;

	if (actual.channels == 1)
	{
		_channelLayout = ChLayout_Mono;
	}
	else if (actual.channels == 2)
	{
		_channelLayout = ChLayout_Stereo;
	}

	_sampleRate = actual.freq;
	_audioDeviceId = audioDeviceId;

	return true;
}

void Speaker::start()
{
	SDL_PauseAudioDevice(_audioDeviceId, 0);
}

void Speaker::audioCallback(void* userdata, uint8_t* stream, int length)
{
	Speaker* speaker = static_cast<Speaker*>(userdata);
	speaker->handleAudioCallback(stream, length);
}

void Speaker::handleAudioCallback(uint8_t* stream, int length)
{
	SDL_memset(stream, 0, length);

	while (length > 0)
	{
		if (_audioBuffeIndex >= _audioDataLength)
		{
			int decodedLength = -1;
			if (_queryAudioData)
			{ 
				decodedLength = _queryAudioData(_audioBuffer, sizeof(_audioBuffer));
			}

			if (decodedLength < 0)
			{
				_audioDataLength = 1024;
				std::memset(_audioBuffer, 0, _audioDataLength);
			}
			else
			{
				_audioDataLength = decodedLength;
			}
			_audioBuffeIndex = 0;
		}

		uint32_t audioChunkLength = _audioDataLength - _audioBuffeIndex;
		if (audioChunkLength > length)
		{
			audioChunkLength = length;
		}

		const uint8_t* src = &_audioBuffer[_audioBuffeIndex];
		SDL_MixAudioFormat(stream, src, _sampleFormatNative, audioChunkLength, SDL_MIX_MAXVOLUME);

		length -= audioChunkLength;
		stream += audioChunkLength;
		_audioBuffeIndex += audioChunkLength;
	}
}
