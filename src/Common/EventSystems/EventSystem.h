#pragma once

#include <map>
#include <list>

class Event;
class EventSystem;

typedef void (*HandleEvent)(EventSystem*, Event, void*);

// 订阅者
struct EventHandler
{
    void* _this;
    HandleEvent func;
};

// 事件类型
class Event
{
public:
    Event(const char* Name, const char* Dest);
    auto operator <=>(const Event&) const = default;

private:
    const char* Name;
    const char* Dest;
};

// 事件管理器
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
    // 事件管理器
    static EventSystem General;
    static EventSystem Render;
    static EventSystem Logic;
    static EventSystem SaveLoad;
};

class Events
{
public:
    // 游戏进程事件
    static Event ScenarioClearClassesEvent;
    static Event ScenarioStartEvent;
    // 渲染事件
    static Event GScreenRenderEvent;
    static Event SidebarRenderEvent;
    // 单位逻辑事件
    static Event LogicUpdateEvent;
    // 游戏保存事件
    static Event SaveGameEvent;
    // 游戏读取事件
    static Event LoadGameEvent;
};
