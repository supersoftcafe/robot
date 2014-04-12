
#ifndef __POLLEVENTQUEUE_H
#define __POLLEVENTQUEUE_H

#include "../interfaces/IEventQueue.h"

class PollEventQueue : public IEventQueue
{
public:
    static std::shared_ptr<PollEventQueue> Create();

protected:
    PollEventQueue();
    virtual ~PollEventQueue();
};

#endif // __POLLEVENTQUEUE_H

