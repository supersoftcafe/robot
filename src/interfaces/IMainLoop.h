
#ifndef __IMAINLOOP_H
#define __IMAINLOOP_H

#include <memory>

#include "../tools/Exception.h"

class MainLoopException : public Exception
{
public:
    MainLoopException(const char* message);
};


class IMainLoop
{
public:
    void Loop();
    void Exit();

protected:
    IMainLoop();
    virtual ~IMainLoop();

private:
    virtual void DoLoop() = 0;
    virtual void DoExit() = 0;
};

#endif // __IMAINLOOP_H

