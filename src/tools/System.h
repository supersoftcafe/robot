
#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <memory>
#include <cstdint>
#include <exception>

class SystemException : public std::exception
{
public:
    SystemException(const char* message);

    virtual const char* what() const throw();

private:
    const char* what_;
};

class ITime;
class ITimer;
class ITimerFactory;
class IFileWatcher;
class IFileWatcherFactory;
class IEvent;
class IEventQueue;
class IMainLoop;
class System
{
public:
    static uint64_t CurrentTimeMS();
    static std::shared_ptr<ITimer> CreateTimer();
    static std::shared_ptr<IFileWatcher> CreateFileWatcher();
    static void PostEvent(const std::shared_ptr<IEvent>&);
    static void Exit();

    static void set_main_loop(           const std::shared_ptr<IMainLoop          >& value);
    static void set_time(                const std::shared_ptr<ITime              >& value);
    static void set_timer_factory(       const std::shared_ptr<ITimerFactory      >& value);
    static void set_file_watcher_factory(const std::shared_ptr<IFileWatcherFactory>& value);
    static void set_event_queue(         const std::shared_ptr<IEventQueue        >& value);

private:
    System();
    ~System();
};

#endif // __SYSTEM_H

