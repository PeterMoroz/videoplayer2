#pragma once

class VideoOutputDevice final
{
public:
	VideoOutputDevice() = default;
	~VideoOutputDevice() = default;

	VideoOutputDevice(const VideoOutputDevice&) = delete;
	VideoOutputDevice& operator=(const VideoOutputDevice&) = delete;
};
