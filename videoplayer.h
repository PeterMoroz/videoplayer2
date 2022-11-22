#pragma once

class AudioOutputDevice;
class VideoOutputDevice;

class Videoplayer final
{
public:
	Videoplayer(VideoOutputDevice& videoOutputDevice, AudioOutputDevice& audioOutputDevice);
	~Videoplayer();

	Videoplayer(const Videoplayer&) = delete;
	Videoplayer& operator=(const Videoplayer&) = delete;

	void Open(const char* uri);
	void Play();
	void Stop();


private:
	VideoOutputDevice& _videoOutputDevice;
	AudioOutputDevice& _audioOutputDevice;
};
