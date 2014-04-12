
#ifndef __SELECTMAINLOOP_H
#define __SELECTMAINLOOP_H

#include <memory>
#include "../interfaces/IMainLoop.h"
#include "../interfaces/IFileWatcher.h"
#include "../interfaces/ITimer.h"
#include "../interfaces/ITime.h"


class SelectMainLoop : public IMainLoop, public IFileWatcherFactory, public ITimerFactory, public ITime
{
public:
    static std::shared_ptr<SelectMainLoop> Create();

protected:
    SelectMainLoop();
    ~SelectMainLoop();
};

#endif // __SELECTMAINLOOP_H

