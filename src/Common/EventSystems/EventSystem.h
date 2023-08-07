#pragma once

#include <map>
#include <list>

using namespace std;

namespace Extension {
    namespace EventSystems
    {
        class EventBase;

        struct EventHandler;

        class EventSystem
        {
        public:
            void AddHandler(EventBase e, EventHandler handler);
            void RemoveHandler(EventBase e, EventHandler handler);
            void Broadcast(EventBase e, void* args);

        private:
            map<EventBase, list<EventHandler>> _handlers;
        };
    };
};
