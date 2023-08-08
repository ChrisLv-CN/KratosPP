#pragma once

#include <map>
#include <list>

class Event;
class EventSystem;

typedef void (*HandleEvent)(EventSystem*, Event, void*);

// ������
struct EventHandler
{
    void* _this;
    HandleEvent func;
};

// �¼�����
class Event
{
public:
    Event(const char* Name, const char* Dest);
    auto operator <=>(const Event&) const = default;

private:
    const char* Name;
    const char* Dest;
};

// �¼�������
class EventSystem
{
public:
    void AddHandler(Event e, HandleEvent func);
    void AddHandler(Event e, EventHandler handler);
    void RemoveHandler(Event e, HandleEvent func);
    void RemoveHandler(Event e, EventHandler handler);

    void Broadcast(Event e, void* args);

private:
    std::map<Event, std::list<EventHandler>> _handlers;
};


class EventSystems
{
public:
    // �¼�������
    static EventSystem General;
    static EventSystem Render;
    static EventSystem Logic;
};

class Events
{
public:
    // ��Ϸ�����¼�
    static Event ScenarioClearClassesEvent;
    static Event ScenarioStartEvent;
    // ��Ⱦ�¼�
    static Event GScreenRenderEvent;
    static Event SidebarRenderEvent;
    // ��λ�߼��¼�
    static Event LogicUpdateEvent;
};