#include "event_dispatcher.h"
#include "sdl_library.h"

#include <iostream>
#include <iomanip>

#include <SDL.h>

namespace
{

	const char* getEventName(EventDispatcher::EventId eventId)
	{
		switch (eventId)
		{
		case EventDispatcher::Evt_Quit:
			return "Event_Quit";
		default:
			return "<unknown>";
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

void EventDispatcher::pollEvents()
{
	SDL_Event event;
	EventId eventId = Evt_Unknown;
	int res = SDL_PollEvent(&event);
	if (res != 0)
	{
		switch (event.type)
		{
		case SDL_QUIT:
			eventId = Evt_Quit;
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
		//else
		//{
		//	std::cerr << "Unknown event (SDL) 0x" 
		//		<< std::hex << std::setw(4) << std::setfill('0') << event.type << std::endl;
		//}
	}
}

bool EventDispatcher::addHandler(EventId eventId, EventHandler&& handler)
{
	auto res = _handlers.emplace(std::make_pair(eventId, std::move(handler)));
	return res.second;
}
