#include "picture_writer.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

void PictureWriter::savePicture(uint8_t* data[4], int linesize[4], int width, int height)
{
	std::ofstream ofs;
	std::ostringstream oss;

	if (_pictureCount < _maxPictureCount)
	{
		oss << "frame-" << std::setw(4) << std::setfill('0') << _pictureCount << ".ppm";
		_pictureCount++;

		ofs.open(oss.str().c_str(), std::ios::binary | std::ios::out);
		if (!ofs.is_open())
		{
			std::cerr << "Could not open file " << oss.str() << " to write." << std::endl;
			return;
		}

		oss.str(std::string());
		oss << "P6\n" << width << ' ' << height << "\n255\n";
		ofs.write(oss.str().data(), oss.str().length());

		const char* pixeldata = reinterpret_cast<char*>(data[0]);
		for (int y = 0; y < height; y++)
			ofs.write(pixeldata + y * linesize[0], width * 3);
	}


}