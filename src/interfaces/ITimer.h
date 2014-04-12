
#ifndef __ITIMER_H
#define __ITIMER_H

#include <memory>
#include <cstdint>
#include <exception>

#include "../interfaces/IGenericService.h"


class TimerException : public std::exception
{
public:
    TimerException(const char* message);

    virtual const char* what() const throw();

private:
    const char* what_;
};


class ITimer;


class ITimerDelegate
{
public:
    void TimerUpdate(ITimer& timer, int data);

protected:
    ITimerDelegate();
    ~ITimerDelegate();

private:
    virtual void DoTimerUpdate(ITimer& timer, int data) = 0;
};


class ITimerFactory
{
public:
    std::shared_ptr<ITimer> CreateTimer();

protected:
    ITimerFactory();
    virtual ~ITimerFactory();

private:
    virtual std::shared_ptr<ITimer> DoCreateTimer() = 0;
};


class ITimer : public IGenericService
{
public:
    void set_delegate(const std::shared_ptr<ITimerDelegate>& value);
    std::shared_ptr<ITimerDelegate> delegate();

    void set_delegate_data(int value);
    int delegate_data();

    void set_timeout_ms(uint32_t value);
    uint32_t timeout_ms();

    void set_repeat(bool value);
    bool repeat();

protected:
    ITimer();
    ~ITimer();

private:
    virtual void do_set_delegate(const std::shared_ptr<ITimerDelegate>& value) = 0;
    virtual std::shared_ptr<ITimerDelegate> do_delegate() = 0;

    virtual void do_set_delegate_data(int data) = 0;
    virtual int do_delegate_data() = 0;

    virtual void do_set_timeout_ms(uint32_t value) = 0;
    virtual uint32_t do_timeout_ms() = 0;

    virtual void do_set_repeat(bool value) = 0;
    virtual bool do_repeat() = 0;
};

#endif // __ITIMER_H

