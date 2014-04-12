
#include "../interfaces/ITime.h"

ITime::ITime()
{
}

ITime::~ITime()
{
}

uint64_t ITime::CurrentTimeMS()
{
    return DoCurrentTimeMS();
}

