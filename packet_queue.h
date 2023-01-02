#pragma once

#include <cstddef>
#include <memory>


struct AVPacketList;
struct AVPacket;

struct SDL_mutex;
struct SDL_cond;

class PacketQueue final
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
	PacketQueue();
	~PacketQueue();

	bool enqueue(AVPacket* packet);
	bool dequeue(AVPacket* packet);

	PacketQueue(const PacketQueue&) = delete;
	PacketQueue& operator=(const PacketQueue&) = delete;

private:
	int _packetsCount = 0;
	int _size = 0;
	//SDL_mutex* _mutex = NULL;
	//SDL_cond* _cond = NULL;

	using MutexDeleter = void(*)(SDL_mutex*);
	using CondDeleter = void(*)(SDL_cond*);

	std::unique_ptr<SDL_mutex, MutexDeleter> _mutex;
	std::unique_ptr<SDL_cond, CondDeleter> _cond;

	AVPacketList* _first_pkt = NULL;
	AVPacketList* _last_pkt = NULL;
};
