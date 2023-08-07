#include <pthread.h>

#include <queue>


class EventSystem
{
public:
    typedef void (*HandleEvent)(void*, void*);

    pthread_mutex_t _mutexSubscriber;
    pthread_mutex_t _mutexPublish;

    struct EventNode
    {
        void* _this;
        HandleEvent func;
    };

    struct EventPublishNode
    {
        char* event;
        void* data;
    };

public:
    EventSystem()
    {
        pthread_mutex_init(&_mutexSubscriber, nullptr);
    }

};
