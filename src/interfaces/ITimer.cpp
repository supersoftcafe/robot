
#include <iostream>

#include "../interfaces/ITimer.h"


TimerException::TimerException(const char* message) : what_(message)
{
}

const char* TimerException::what() const throw()
{
    return what_;
}


ITimerFactory::ITimerFactory()
{
}

ITimerFactory::~ITimerFactory()
{
}

std::shared_ptr<ITimer> ITimerFactory::CreateTimer()
{
    return DoCreateTimer();
}


ITimerDelegate::ITimerDelegate()
{
}

ITimerDelegate::~ITimerDelegate()
{
}

void ITimerDelegate::TimerUpdate(ITimer& timer, int data)
{
    DoTimerUpdate(timer, data);
}


ITimer::ITimer()
{
}

ITimer::~ITimer()
{
}

void ITimer::set_delegate(const std::shared_ptr<ITimerDelegate>& value)
{
    do_set_delegate(value);
}

std::shared_ptr<ITimerDelegate> ITimer::delegate()
{
    return do_delegate();
}

void ITimer::set_delegate_data(int value)
{
    do_set_delegate_data(value);
}

int ITimer::delegate_data()
{
    return do_delegate_data();
}

void ITimer::set_timeout_ms(uint32_t value)
{
    if (value == 0)
    {
        std::cerr << "ITimer::set_timeout_ms() with zero timeout" << std::endl;
        throw TimerException("ITimer::set_timeout_ms() with zero timeout");
    }
    do_set_timeout_ms(value);
}

uint32_t ITimer::timeout_ms()
{
    return do_timeout_ms();
}

void ITimer::set_repeat(bool value)
{
    do_set_repeat(value);
}

bool ITimer::repeat()
{
    return do_repeat();
}



