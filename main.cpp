#include <cstdlib>
#include <iostream>
#include <sstream>

#include "videoplayer.h"
#include "audio_output_device.h"
#include "display.h"

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <filename> " << std::endl;
		std::exit(EXIT_FAILURE);
	}

	try {
		AudioOutputDevice audioOutputDevice;
		Display display;

		std::ostringstream oss;
		oss << "videplayer - " << argv[0];
		if (display.addWindow(oss.str().c_str(), 10, 10, 1024, 768, VideoOutputDevice::PixFormat_RGB24))
		{
			std::cerr << "Could not add render window. " << std::endl;
			std::exit(EXIT_FAILURE);
		}

		Videoplayer videoplayer(display, audioOutputDevice);

		videoplayer.Open(argv[1]);
		videoplayer.Play();
	}
	catch (const std::exception& ex) {
		std::cerr << "Exception: " << ex.what() << "\nLocation: "
			<< __FUNCTION__ << ", " << __FILE__ << ", " << __LINE__ << std::endl;
		std::exit(EXIT_FAILURE);
	}

	std::exit(EXIT_SUCCESS);
	return 0;
}