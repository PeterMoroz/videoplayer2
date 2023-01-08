#include "event_dispatcher.h"
#include "sdl_library.h"

#include <cassert>
#include <iostream>

#include <SDL.h>

namespace
{

	const char* getEventName(EventDispatcher::EventId eventId)
	{
		switch (eventId)
		{
		case EventDispatcher::Evt_Quit:
			return "Event_Quit";
		case EventDispatcher::Evt_RefreshScreen:
			return "Event_RefreshScreen";
		default:
			assert(false);
		}
		return "";
	}

}


EventDispatcher::EventDispatcher()
{
	if (!SDL_Library::getInstance().initSubsystem(SDL_INIT_EVENTS))
	{
		std::cerr << "SDL init events failed." << std::endl;
		throw std::runtime_error("Can not initialize events subsystem (SDL).");
	}
}

EventDispatcher::~EventDispatcher()
{
	SDL_Library::getInstance().quitSubsystem(SDL_INIT_EVENTS);
}

EventDispatcher& EventDispatcher::getInstance()
{
	static EventDispatcher instance;
	return instance;
}

void EventDispatcher::processEvents()
{
	while (1)
	{
		SDL_Event event;
		EventId eventId = Evt_Unknown;
		int res = SDL_WaitEvent(&event);
		if (res == 0 || res == 1)
		{
			switch (event.type)
			{
			case SDL_QUIT:
				eventId = Evt_Quit;
				break;
			case SDL_USEREVENT + 1:	// UGLY!
				eventId = Evt_RefreshScreen;
				break;
			default:
				;
			}

			if (eventId != Evt_Unknown)
			{
				auto it = _handlers.find(eventId);
				if (it != _handlers.cend())
				{
					it->second();
				}
				else
				{
					std::cerr << "No handler for event " << getEventName(eventId) << std::endl;
				}
			}
		}
	}
}

bool EventDispatcher::addHandler(EventId eventId, EventHandler&& handler)
{
	auto res = _handlers.emplace(std::make_pair(eventId, std::move(handler)));
	return res.second;
}

bool EventDispatcher::pushEvent(EventId eventId, void* userdata)
{
	SDL_Event event;

	switch (eventId)
	{
	case EventDispatcher::Evt_Quit:
		event.type = SDL_QUIT;
		break;
	case EventDispatcher::Evt_RefreshScreen:
		event.type = SDL_USEREVENT + 1;
		break;
	default:
		std::cerr << "Unknown event " << static_cast<int>(eventId) << std::endl;
		assert(false);
	}

	event.user.data1 = userdata;

	int res = SDL_PushEvent(&event);
	if (res != 1)
	{
		std::cerr << "SDL_PushEvent() failed. error: " << SDL_GetError() << std::endl;
		return false;
	}
	return true;
}
