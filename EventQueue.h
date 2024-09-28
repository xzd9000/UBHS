#pragma once
#include<forward_list>
#include"Event.h"

struct EventQueue
{
	std::forward_list<Event> q;
	inline void SortEventQueue() { q.sort([](Event e1, Event e2) -> bool { return e1.activationTime < e2.activationTime; }); }
	inline void AddEvent(Event e) { q.push_front(e); SortEventQueue(); }
};
