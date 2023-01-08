#include "packet_queue.h"

#include <SDL.h>

extern "C"
{
#include <libavformat/avformat.h>
}

#include <iostream>


PacketQueue::Lock::Lock(SDL_mutex* mutex)
	: _mutex(mutex)
{
	if (SDL_LockMutex(_mutex) < 0)
	{
		std::cerr << "SDL_LockMutex() failed. error: " << SDL_GetError() << std::endl;
	}
	else
	{
		_locked = true;
	}
}

PacketQueue::Lock::~Lock()
{
	if (_locked)
	{
		if (SDL_UnlockMutex(_mutex) < 0)
		{
			std::cerr << "SDL_UnlockMutex() failed. error: " << SDL_GetError() << std::endl;
		}
	}
}

PacketQueue::PacketQueue()
	: _mutex(NULL, &SDL_DestroyMutex)
	, _cond(NULL, &SDL_DestroyCond)
{
	std::unique_ptr<SDL_mutex, MutexDeleter> mutex(SDL_CreateMutex(), &SDL_DestroyMutex);
	if (!mutex)
	{
		std::cerr << "SDL_CreateMutex() failed. error: " << SDL_GetError() << std::endl;
		throw std::runtime_error("Couldn't create SDL mutex.");
	}

	std::unique_ptr<SDL_cond, CondDeleter> cond(SDL_CreateCond(), &SDL_DestroyCond);
	if (!cond)
	{
		std::cerr << "SDL_CreateCond() failed. error: " << SDL_GetError() << std::endl;
		throw std::runtime_error("Couldn't create SDL cond.");
	}

	_mutex.swap(mutex);
	_cond.swap(cond);
}

PacketQueue::~PacketQueue()
{

}

bool PacketQueue::enqueue(AVPacket* packet)
{
	if (av_dup_packet(packet) < 0)
	{
		std::cerr << "av_dup_packet() failed. " << std::endl;
		return false;
	}

	AVPacketList* pktl = static_cast<AVPacketList*>(av_malloc(sizeof(AVPacketList)));
	if (!pktl)
	{
		std::cerr << "av_malloc() failed. " << std::endl;
		return false;
	}

	Lock lock(_mutex.get());
	if (!lock.locked())
	{
		av_free(pktl);
		return false;
	}

	pktl->pkt = *packet;
	pktl->next = NULL;

	if (!_first_pkt)
		_first_pkt = pktl;
	else
		_last_pkt->next = pktl;

	_last_pkt = pktl;
	_packetsCount++;
	_size += pktl->pkt.size;

	if (SDL_CondSignal(_cond.get()) != 0)
	{
		std::cerr << "SDL_CondSignal() failed. error: " << SDL_GetError() << std::endl;
		return false;
	}

	return true;
}

bool PacketQueue::dequeue(AVPacket* packet)
{
	bool ret = true;
	AVPacketList* pktl = NULL;

	Lock lock(_mutex.get());
	if (!lock.locked())
	{
		return false;
	}

	for (;;)
	{
		pktl = _first_pkt;
		if (pktl)
		{
			_first_pkt = pktl->next;
			if (!_first_pkt)
				_last_pkt = NULL;
			_packetsCount--;
			_size -= pktl->pkt.size;
			*packet = pktl->pkt;
			av_free(pktl);
			return true;
		}

		if (SDL_CondWait(_cond.get(), _mutex.get()) != 0)
		{
			std::cerr << "SDL_CondWait() failed. error: " << SDL_GetError() << std::endl;
			return false;
		}
	}

	// unreachable
	return true;
}
