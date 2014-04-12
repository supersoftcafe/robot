
#ifndef __ITIME_H
#define __ITIME_H

#include <memory>
#include <cstdint>

class ITime
{
public:
    uint64_t CurrentTimeMS();

protected:
    ITime();
    virtual ~ITime();

private:
    virtual uint64_t DoCurrentTimeMS() = 0;
};

#endif

