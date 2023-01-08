#pragma once

#include <cstdint>

struct SDL_mutex;
struct SDL_cond;

class ImageBuffer final
{
public:
	ImageBuffer(const ImageBuffer&) = delete;
	ImageBuffer& operator=(const ImageBuffer&) = delete;

	ImageBuffer();
	~ImageBuffer();

	bool allocate(int width, int height, int format, int align = 1);
	void release();

	bool acquireWrite(uint8_t* image_data[], int linesize[]);
	void releaseWrite();

	void acquireRead(uint8_t* image_data[], int linesize[]);
	bool releaseRead();

	bool empty() const { return _empty; }

private:
	uint8_t* _image_data[4] = { NULL };
	int _linesize[4] = { 0 };
	bool _empty = true;

	SDL_mutex* _mutex = NULL;
	SDL_cond* _cond = NULL;
};
