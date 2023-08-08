#include <Common/EventSystems/EventSystem.h>

Event::Event(const char* Name, const char* Dest)
{
    this->Name = Name;
    this->Dest = Dest;
}

void EventSystem::AddHandler(Event e, HandleEvent func)
{
    EventHandler h{func, func};
    this->AddHandler(e, h);
}

void EventSystem::AddHandler(Event e, EventHandler handler)
{
    this->_handlers[e].push_back(handler);
}

void EventSystem::RemoveHandler(Event e, HandleEvent func)
{
    EventHandler h{ func, func };
    this->RemoveHandler(e, h);
}

void EventSystem::RemoveHandler(Event e, EventHandler handler)
{
    auto it = this->_handlers.find(e);
    if (it != this->_handlers.end())
    {
        for (auto ite = it->second.begin(); ite != it->second.end();)
        {
            if (ite->_this == handler._this && ite->func == handler.func)
            {
                ite = it->second.erase(ite);
            }
            else
            {
                ++ite;
            }
        }
    }
}


void EventSystem::Broadcast(Event e, void* args)
{
    auto it = this->_handlers.find(e);
    if (it != this->_handlers.end())
    {
        for (auto ite = it->second.begin(); ite != it->second.end(); ++ite)
        {
            ite->func(this, e, args);
        }
    }
}

// 事件管理器
EventSystem EventSystems::General;
EventSystem EventSystems::Render;
EventSystem EventSystems::Logic;

// 进程事件
Event Events::ScenarioClearClassesEvent = Event("ScenarioClearClasses", "Raised when scenario is cleaning classes");
Event Events::ScenarioStartEvent = Event("ScenarioStart", "Raised when scenario start");
// 渲染事件
Event Events::GScreenRenderEvent = Event("GScreenClass_Render", "Raised when GScreen is Render");
Event Events::SidebarRenderEvent = Event("SidebarClass_Draw_It", "Raised when Sidebar is Render");
// 逻辑事件
Event Events::LogicUpdateEvent = Event("LogicClassUpdate", "Raised when LogicClass update");
