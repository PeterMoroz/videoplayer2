#pragma once

#include <functional>
#include <unordered_map>

class EventDispatcher final
{
public:
	enum EventId
	{
		Evt_Unknown = 0,
		Evt_Quit,
		Evt_RefreshScreen,
		Evt_DemuxFinished,
	};

	using EventHandler = std::function<void(void)>;

public:
	~EventDispatcher();
	static EventDispatcher& getInstance();

	void processEvents();
	bool addHandler(EventId eventId, EventHandler&& handler);
	bool pushEvent(EventId eventId, void* userdata = NULL);

	void stopEventProcessing();

private:
	EventDispatcher();

	EventDispatcher(const EventDispatcher&) = delete;
	EventDispatcher& operator=(const EventDispatcher&) = delete;

private:
	std::unordered_map<EventId, EventHandler> _handlers;
	bool _processEvents = false;
};
