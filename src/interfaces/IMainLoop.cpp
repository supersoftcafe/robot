
#include "../interfaces/ITimer.h"
#include "../interfaces/IFileWatcher.h"
#include "../interfaces/IMainLoop.h"



MainLoopException::MainLoopException(const char* message) : Exception(message)
{
}


IMainLoop::IMainLoop()
{
}

IMainLoop::~IMainLoop()
{
}

void IMainLoop::Loop()
{
    DoLoop();
}

void IMainLoop::Exit()
{
    DoExit();
}


