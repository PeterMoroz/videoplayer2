#include <cstdlib>
#include <iostream>

#include "videoplayer.h"
#include "audio_output_device.h"
#include "video_output_device.h"

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <filename> " << std::endl;
		std::exit(EXIT_FAILURE);
	}

	try {
		AudioOutputDevice audioOutputDevice;
		VideoOutputDevice videoOutputDevice;

		Videoplayer videoplayer(videoOutputDevice, audioOutputDevice);

		videoplayer.Open(argv[1]);
		videoplayer.Play();
	}
	catch (const std::exception& ex) {
		std::cerr << "Exception: " << ex.what() << "\nLocation:"
			<< __FUNCTION__ << ", " << __FILE__ << ", " << __LINE__ << std::endl;
		std::exit(EXIT_FAILURE);
	}

	std::exit(EXIT_SUCCESS);
	return 0;
}