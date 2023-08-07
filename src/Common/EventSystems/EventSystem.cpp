#include <Common/EventSystems/EventSystem.h>


void EventSystem::AddHandler(EventBase e, EventHandler handler)
{
    EventSystem::_handlers[e].push_back(handler);
}

void EventSystem::RemoveHandler(EventBase e, EventHandler handler)
{
    auto it = EventSystem::_handlers.find(e);
    if (it != EventSystem::_handlers.end())
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

void EventSystem::Broadcast(EventBase e, void* args)
{
    auto it = EventSystem::_handlers.find(e);
    if (it != EventSystem::_handlers.end())
    {
        for (auto ite = it->second.begin(); ite != it->second.end(); ++ite)
        {
            ite->func(ite->_this, args);
        }
    }
}
