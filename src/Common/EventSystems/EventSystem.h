#pragma once
#ifndef EventSystem_H
#define EventSystem_H

#include <map>
#include <list>

typedef void (*HandleEvent)(void*, void*);

class EventBase
{
    const char* Name;
    const char* Dest;

public:
    auto operator <=>(const EventBase&) const = default;
};

struct EventHandler
{
    void* _this;
    HandleEvent func;
};

class EventSystem
{
public:
    void AddHandler(EventBase e, EventHandler handler);
    void RemoveHandler(EventBase e, EventHandler handler);
    void Broadcast(EventBase e, void* args);

private:
    std::map<EventBase, std::list<EventHandler>> _handlers;
};

#endif // !EventSystem_H
