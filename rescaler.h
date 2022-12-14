#pragma once

#include <cstdint>

struct SwsContext;

struct AVFrame;

class PictureWriter;
class ImageBuffer;


class Rescaler final
{
public:
	enum Flags
	{
		FastBilinear = 0x1,
		Bilinear = 0x2,
		Bicubic = 0x2,
		X = 0x8,
		Point = 0x10,
		Area = 0x20,
		Bicublin = 0x40,
		Gauss = 0x80,
		Sinc = 0x100,
		Lanczos = 0x200,
		Spline = 0x400,
		PrintInfo = 0x1000,
	};

public:
	Rescaler();
	~Rescaler();

	Rescaler(const Rescaler&) = delete;
	Rescaler& operator=(const Rescaler&) = delete;

	bool init(int srcWidth, int srcHeight, int srcFormat, 
			int dstWidth, int dstHeight, int dstFormat, int flags);

	// void setPictureBuffer(uint8_t* pictureData[4], int pictureLinesize[4]);
	void setOutputBuffer(ImageBuffer* imageBuffer);
	void setPictureWriter(PictureWriter* writer);

	void scaleFrame(const AVFrame* frame);

private:
	SwsContext *_context;
	int _dstWidth = 0;
	int _dstHeight = 0;

	uint8_t* _pictureData[4];
	int _pictureLinesize[4] = { 0 };
	PictureWriter* _pictureWriter;
};
