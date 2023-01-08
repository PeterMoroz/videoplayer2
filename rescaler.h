#pragma once

#include <cstdint>

struct SwsContext;

struct AVFrame;


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

	void scaleFrame(const AVFrame* frame);

	void setOutputBuffer(uint8_t* pixeldata[], int linesize []);

private:
	SwsContext *_context;
	int _dstWidth = 0;
	int _dstHeight = 0;

	uint8_t* _pixeldata[4] = { NULL };
	int _linesize[4] = { 0 };
};
