
#include <chrono>
#include <cstring>
#include <iostream>

#include "../interfaces/IMainLoop.h"
#include "../interfaces/IEventQueue.h"
#include "../interfaces/IFileWatcher.h"
#include "../interfaces/ITimer.h"
#include "../interfaces/ITime.h"
#include "../tools/System.h"



SystemException::SystemException(const char* message) : what_(message)
{
}

const char* SystemException::what() const throw()
{
    return what_;
}


namespace { // Start of anonymous

std::shared_ptr<ITime> time_source;
std::shared_ptr<ITimerFactory> timer_factory;
std::shared_ptr<IFileWatcherFactory> file_watcher_factory;
std::shared_ptr<IEventQueue> event_queue;
std::shared_ptr<IMainLoop> main_loop;

} // End of anonymous


void System::set_time(const std::shared_ptr<ITime>& value)
{
    if (time_source)
    {
        std::cerr << "System::set_time called twice" << std::endl;
        throw SystemException("System::set_time called twice");
    }
    time_source = value;
}

void System::set_timer_factory(const std::shared_ptr<ITimerFactory>& value)
{
    if (timer_factory)
    {
        std::cerr << "System::set_timer_factory called twice" << std::endl;
        throw SystemException("System::set_timer_factory called twice");
    }
    timer_factory = value;
}

void System::set_file_watcher_factory(const std::shared_ptr<IFileWatcherFactory>& value)
{
    if (file_watcher_factory)
    {
        std::cerr << "System::set_file_watcher_factory called twice" << std::endl;
        throw SystemException("System::set_file_watcher_factory called twice");
    }
    file_watcher_factory = value;
}

void System::set_event_queue(const std::shared_ptr<IEventQueue>& value)
{
    if (event_queue)
    {
        std::cerr << "System::set_event_queue called twice" << std::endl;
        throw SystemException("System::set_event_queue called twice");
    }
    event_queue = value;
}

void System::set_main_loop(const std::shared_ptr<IMainLoop>& value)
{
    if (main_loop)
    {
        std::cerr << "System::set_main_loop called twice" << std::endl;
        throw SystemException("System::set_main_loop called twice");
    }
    main_loop = value;
}

uint64_t System::CurrentTimeMS()
{
    if (!time_source)
    {
        std::cerr << "System::CurrentTimeMS called before set_time" << std::endl;
        throw SystemException("System::CreateTimer called before set_time");
    }
    return time_source->CurrentTimeMS();
}

std::shared_ptr<ITimer> System::CreateTimer()
{
    if (!timer_factory)
    {
        std::cerr << "System::CreateTimer called before set_timer_factory" << std::endl;
        throw SystemException("System::CreateTimer called before set_timer_factory");
    }
    return timer_factory->CreateTimer();
}

std::shared_ptr<IFileWatcher> System::CreateFileWatcher()
{
    if (!file_watcher_factory)
    {
        std::cerr << "System::CreateFileWatcher called before set_file_watcher_factory" << std::endl;
        throw SystemException("System::CreateFileWatcher called before set_file_watcher_factory");
    }
    return file_watcher_factory->CreateFileWatcher();
}

void System::PostEvent(const std::shared_ptr<IEvent>& event)
{
    if (!event_queue)
    {
        std::cerr << "System::PostEvent called before set_event_queue" << std::endl;
        throw SystemException("System::PostEvent called before set_event_queue");
    }
    event_queue->PostEvent(event);
}

void System::Exit()
{
    if (!main_loop)
    {
        std::cerr << "System::Exit called before set_main_loop" << std::endl;
        throw SystemException("System::Exit called before set_main_loop");
    }
    main_loop->Exit();
}


