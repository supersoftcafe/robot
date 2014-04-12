
#include <iostream>

#include "../interfaces/IEventQueue.h"


EventQueueException::EventQueueException(const char* message) : what_(message)
{
}

const char* EventQueueException::what() const throw()
{
    return what_;
}


IEvent::IEvent()
{
}

IEvent::~IEvent()
{
}

void IEvent::Run()
{
    DoRun();
}


IEventQueue::IEventQueue()
{
}

IEventQueue::~IEventQueue()
{
}

void IEventQueue::PostEvent(const std::shared_ptr<IEvent>& event)
{
    if (!event)
    {
        std::cerr << "IEventQueue::PostEvent with null event" << std::endl;
        throw EventQueueException("IEventQueue::PostEvent with null event");
    }
    DoPostEvent(event);
}

void IEventQueue::PostEvent(std::shared_ptr<IEvent>&& event)
{
    if (!event)
    {
        std::cerr << "IEventQueue::PostEvent with null event" << std::endl;
        throw EventQueueException("IEventQueue::PostEvent with null event");
    }
    DoPostEvent(event);
}


