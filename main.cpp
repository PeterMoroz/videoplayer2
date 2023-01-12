#include <cstdlib>
#include <iostream>
#include <sstream>

#include "videoplayer.h"
#include "speaker.h"
#include "display.h"
#include "event_dispatcher.h"

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <filename> " << std::endl;
		std::exit(EXIT_FAILURE);
	}

	// putenv("SDL_AUDIODRIVER=DirectSound");

	try {
		Speaker speaker;
		Display display;

		static const int WindowOriginX = 100;
		static const int WindowOriginY = 100;

		static const int WindowWidth = 1024;
		static const int WindowHeight = 768;

		std::ostringstream oss;
		oss << "videplayer - " << argv[0];
		if (display.addWindow(oss.str().c_str(), WindowOriginX, WindowOriginY, 
			WindowWidth, WindowHeight, VideoOutputDevice::PixFormat_RGB24) < 0)
		{
			std::cerr << "Could not add render window. " << std::endl;
			std::exit(EXIT_FAILURE);
		}

		static const int AudioSampleRate = 48000;
		static const int AudioChannelsNumber = 2;
		static const int AudioBufferSize = 2048;

		if (!speaker.init(AudioSampleRate, AudioChannelsNumber, AudioBufferSize, AudioOutputDevice::SampleFormat_S16SYS))
		{
			std::cerr << "Could not init speaker." << std::endl;
			std::exit(EXIT_FAILURE);
		}

		Videoplayer videoplayer(display, speaker);

		videoplayer.Open(argv[1]);
		videoplayer.Play();
		EventDispatcher::getInstance().processEvents();
	}
	catch (const std::exception& ex) {
		std::cerr << "Exception: " << ex.what() << "\nLocation: "
			<< __FUNCTION__ << ", " << __FILE__ << ", " << __LINE__ << std::endl;
		std::exit(EXIT_FAILURE);
	}

	std::exit(EXIT_SUCCESS);
	return 0;
}