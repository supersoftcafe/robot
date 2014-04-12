
#ifndef __IEVENTQUEUE_H
#define __IEVENTQUEUE_H

#include <memory>
#include <exception>

class EventQueueException : public std::exception
{
public:
    EventQueueException(const char* message);

    virtual const char* what() const throw();

private:
    const char* what_;
};

class IEvent
{
public:
    void Run();

protected:
    IEvent();
    virtual ~IEvent();

private:
    virtual void DoRun() = 0;
};


class IEventQueue
{
public:
    void PostEvent(const std::shared_ptr<IEvent>&);
    void PostEvent(std::shared_ptr<IEvent>&&);

protected:
    IEventQueue();
    virtual ~IEventQueue();

private:
    virtual void DoPostEvent(const std::shared_ptr<IEvent>&) = 0;
    virtual void DoPostEvent(std::shared_ptr<IEvent>&&) = 0;
};

#endif // __IEVENTQUEUE_H

