#pragma once

#include <cstdint>
#include <memory>

struct SDL_mutex;
struct SDL_cond;

class ImageBuffer final
{
private:
	class Lock
	{
	public:
		explicit Lock(SDL_mutex* mutex);
		~Lock();

		bool locked() const { return _locked; }

	private:
		SDL_mutex* _mutex;
		bool _locked = false;
	};

public:
	ImageBuffer(const ImageBuffer&) = delete;
	ImageBuffer& operator=(const ImageBuffer&) = delete;

	ImageBuffer();
	~ImageBuffer();

	bool allocate(int width, int height, int format, int align = 1);
	void release();

	bool acquireWrite(uint8_t* image_data[], int linesize[]);
	bool releaseWrite();

	void acquireRead(uint8_t* image_data[], int linesize[]);
	bool releaseRead();

	bool empty() const { return _empty; }
	bool isWaiting() const { return _isWaiting; }

	void setPTS(const double& pts);
	double getPTS() const;

private:
	uint8_t* _image_data[4] = { NULL };
	int _linesize[4] = { 0 };
	bool _empty = true;
	bool _isWaiting = false;

	double _pts = 0.0;

	using MutexDeleter = void(*)(SDL_mutex*);
	using CondDeleter = void(*)(SDL_cond*);

	std::unique_ptr<SDL_mutex, MutexDeleter> _mutex;
	std::unique_ptr<SDL_cond, CondDeleter> _cond;
};
