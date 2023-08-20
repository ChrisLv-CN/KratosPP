#pragma once
#include "MyDelegate.h"

#include <map>
#include <list>

using namespace Delegate;

class EventSystem;
class Event;

extern void* EventArgsLate;
extern void* EventArgsEmpty;

typedef void (*HandleEvent)(EventSystem*, Event, void*);

// 事件类型
class Event
{
public:
    Event(const char* Name, const char* Dest);
    auto operator <=>(const Event&) const = default;

    const char* Name;
    const char* Dest;
};

// 事件管理器
class EventSystem
{
public:
    EventSystem(const char* name);

    void AddHandler(Event e, HandleEvent func);

    template<typename T, typename F>
    void AddHandler(Event e, T* _obj, F func)
    {
        _handlers[e] += newDelegate(_obj, func);
    }

    void RemoveHandler(Event e, HandleEvent func);

    template<typename T, typename F>
    void RemoveHandler(Event e, T* _obj, F* func)
    {
        _handlers[e] -= newDelegate(_obj, func);
    }

    void Broadcast(Event e, void* args = EventArgsEmpty);

    const char* Name;
private:
    std::map<Event, CMultiDelegate<void, EventSystem*, Event, void*>> _handlers;
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
    // 程序生命周期事件
    static Event ExeRun;
    static Event ExeTerminate;
    static Event CmdLineParse;
    // 游戏进程事件
    static Event PointerExpireEvent;
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

