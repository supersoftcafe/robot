
#include "../providers/SelectMainLoop.h"
#include "../interfaces/IFileWatcher.h"
#include "../interfaces/ITimer.h"
#include "../tools/System.h"

#include <list>
#include <chrono>
#include <limits>
#include <cstring>
#include <iostream>
#include <vector>

#include <sys/select.h>




namespace { // start of anonymous

class _Timer;
class _FileWatcher;
class _SelectMainLoop;

class _TimerState
{
public:
    _TimerState() : next_timeout_(0), timeout_ms_(1), repeat_(false), active_(false) { }

    uint64_t                  next_timeout_;
    std::weak_ptr<_Timer>            timer_;
    uint32_t                    timeout_ms_;
    bool                            repeat_;
    bool                            active_;
};

class _Timer : public ITimer
{
public:
    _Timer() : delegate_data_(0) { }

    std::list<_TimerState>::iterator     state_;
    std::shared_ptr<_SelectMainLoop> main_loop_;
    std::weak_ptr<ITimerDelegate>     delegate_;
    int                          delegate_data_;

private:
    virtual void do_set_delegate(const std::shared_ptr<ITimerDelegate>& value) override;
    virtual std::shared_ptr<ITimerDelegate> do_delegate() override;

    virtual void do_set_delegate_data(int data) override;
    virtual int do_delegate_data() override;

    virtual void do_set_timeout_ms(uint32_t value) override;
    virtual uint32_t do_timeout_ms() override;

    virtual void do_set_repeat(bool value) override;
    virtual bool do_repeat() override;

    virtual void DoStart() override;
    virtual void DoStop() override;
};


class _FileWatcherState
{
public:
    _FileWatcherState() : fd_(-1), mode_(IFileWatcher::READ), active_(false) { }

    std::weak_ptr<_FileWatcher> file_watcher_;
    int                                   fd_;
    int                                 mode_;
    bool                              active_;
};

class _FileWatcher : public IFileWatcher
{
public:
    _FileWatcher() : delegate_data_(0) { }

    std::list<_FileWatcherState>::iterator state_;
    std::shared_ptr<_SelectMainLoop>   main_loop_;
    std::weak_ptr<IFileWatcherDelegate> delegate_;
    int                            delegate_data_;

private:
    virtual void do_set_delegate(const std::shared_ptr<IFileWatcherDelegate>& value) override;
    virtual std::shared_ptr<IFileWatcherDelegate> do_delegate() override;

    virtual void do_set_delegate_data(int value) override;
    virtual int do_delegate_data() override;

    virtual void do_set_file_descriptor(int value) override;
    virtual int do_file_descriptor() override;

    virtual void do_set_mode(int value) override;
    virtual int do_mode() override;

    virtual void DoStart() override;
    virtual void DoStop() override;
};


class _SelectMainLoop : public SelectMainLoop, public std::enable_shared_from_this<_SelectMainLoop>
{
public:
    _SelectMainLoop();
    virtual ~_SelectMainLoop();

    std::list<_FileWatcherState> file_watchers_;
    std::list<_TimerState>              timers_;
    bool                          loop_entered_;
    bool                        exit_requested_;

private:
    virtual void DoLoop() override;
    virtual void DoExit() override;

    virtual std::shared_ptr<ITimer> DoCreateTimer() override;
    virtual std::shared_ptr<IFileWatcher> DoCreateFileWatcher() override;

    virtual uint64_t DoCurrentTimeMS() override;
};


void _Timer::do_set_delegate(const std::shared_ptr<ITimerDelegate>& value)
{
    delegate_ = value;
}

std::shared_ptr<ITimerDelegate> _Timer::do_delegate()
{
    return delegate_.lock();
}

void _Timer::do_set_delegate_data(int value)
{
    delegate_data_ = value;
}

int _Timer::do_delegate_data()
{
    return delegate_data_;
}

void _Timer::do_set_timeout_ms(uint32_t value)
{
    state_->timeout_ms_ = value;
}

uint32_t _Timer::do_timeout_ms()
{
    return state_->timeout_ms_;
}

void _Timer::do_set_repeat(bool value)
{
    state_->repeat_ = value;
}

bool _Timer::do_repeat()
{
    return state_->repeat_;
}

void _Timer::DoStart()
{
    state_->next_timeout_ = System::CurrentTimeMS() + state_->timeout_ms_;
    state_->active_ = true;
}

void _Timer::DoStop()
{
    state_->active_ = false;
}


void _FileWatcher::do_set_delegate(const std::shared_ptr<IFileWatcherDelegate>& value)
{
    delegate_ = value;
}

std::shared_ptr<IFileWatcherDelegate> _FileWatcher::do_delegate()
{
    return delegate_.lock();
}

void _FileWatcher::do_set_delegate_data(int value)
{
    delegate_data_ = value;
}

int _FileWatcher::do_delegate_data()
{
    return delegate_data_;
}

void _FileWatcher::do_set_file_descriptor(int value)
{
    if (value < 0) value = -1;
    state_->fd_ = value;
}

int _FileWatcher::do_file_descriptor()
{
    return state_->fd_;
}

void _FileWatcher::do_set_mode(int value)
{
    state_->mode_ = value;
}

int _FileWatcher::do_mode()
{
    return state_->mode_;
}

void _FileWatcher::DoStart()
{
    if (state_->fd_ < 0)
    {
        std::cerr << "_FileWatcher::DoStart() called before do_set_file_descriptor" << std::endl;
        throw FileWatcherException("_FileWatcher::DoStart() called before do_set_file_descriptor");
    }
    state_->active_ = true;
}

void _FileWatcher::DoStop()
{
    state_->active_ = false;
}


_SelectMainLoop::_SelectMainLoop() : loop_entered_(false), exit_requested_(false)
{
}

_SelectMainLoop::~_SelectMainLoop()
{
}

uint64_t _SelectMainLoop::DoCurrentTimeMS()
{
    auto time = std::chrono::steady_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
}

std::shared_ptr<ITimer> _SelectMainLoop::DoCreateTimer()
{
    timers_.emplace_front();
    auto state = timers_.begin(); // _TimerState managed by list
    auto timer = std::make_shared<_Timer>(); // ITimer managed by shared_ptr

    timer->main_loop_ = shared_from_this();
    timer->state_     = state; // Reference to shared internal state
    state->timer_     = timer;

    return std::static_pointer_cast<ITimer>(timer);
}

std::shared_ptr<IFileWatcher> _SelectMainLoop::DoCreateFileWatcher()
{
    file_watchers_.emplace_front();
    auto state = file_watchers_.begin(); // _FileWatcherSTate managed by list
    auto file_watcher = std::make_shared<_FileWatcher>(); // IFileWatcher managed by shared_ptr

    file_watcher->main_loop_ = shared_from_this();
    file_watcher->state_     = state; // Reference to shared internal state
    state->file_watcher_     = file_watcher;

    return std::static_pointer_cast<IFileWatcher>(file_watcher);
}

void _SelectMainLoop::DoExit()
{
    exit_requested_ = true;
}

void _SelectMainLoop::DoLoop()
{
    if (loop_entered_)
    {
        std::cerr << "SelectMainLoop::Loop method re-entered" << std::endl;
        throw MainLoopException("SelectMainLoop::Loop method re-entered");
    }

    loop_entered_ = true;
    try
    {
        while (!exit_requested_)
        {
            fd_set   read_fds;
            fd_set  write_fds;
            fd_set except_fds;

            FD_ZERO(  &read_fds);
            FD_ZERO( &write_fds);
            FD_ZERO(&except_fds);

            // Populate fd sets, and erase stale entries
            int max_fd = -1;
            for (auto fw = file_watchers_.begin(); fw != file_watchers_.end(); )
            {
                if (fw->file_watcher_.expired())
                {
                    fw = file_watchers_.erase(fw);
                }
                else
                {
                    if (fw->fd_ >= 0 && fw->active_)
                    {
                        if ((fw->mode_ & IFileWatcher::READ  ) != 0)
                            FD_SET(fw->fd_, &read_fds);

                        if ((fw->mode_ & IFileWatcher::WRITE ) != 0)
                            FD_SET(fw->fd_, &write_fds);

                        if ((fw->mode_ & IFileWatcher::EXCEPT) != 0)
                            FD_SET(fw->fd_, &except_fds);

                        if (fw->fd_ > max_fd)
                            max_fd = fw->fd_;
                    }
                    ++fw;
                }
            }

            // Calculate appropriate timeout for timers
            uint64_t          now = System::CurrentTimeMS();
            uint64_t next_timeout = now + 60000;
            for (auto ts = timers_.begin(); ts != timers_.end(); )
            {
                if (ts->timer_.expired())
                {
                    ts = timers_.erase(ts);
                }
                else
                {
                    if (ts->active_ && ts->next_timeout_ < next_timeout)
                        next_timeout = ts->next_timeout_;
                    ++ts;
                }
            }
            struct timeval timeout = {0, 0};
            if (next_timeout > now)
            {
                next_timeout = next_timeout - now;
                timeout.tv_sec  = next_timeout / 1000000;
                timeout.tv_usec = next_timeout % 1000000;
            }

            // Wait on select call
            int num_fds = select(max_fd+1, &read_fds, &write_fds, &except_fds, &timeout);
            if (num_fds == -1)
            {
                std::cerr << "select had an error, " << std::strerror(errno) << std::endl;
                throw MainLoopException("select had an error");
            }

            // Test for file events, and don't erase stale elements
            for (auto fw = file_watchers_.begin(); fw != file_watchers_.end(); ++fw)
            {
                if (fw->fd_ >= 0 && fw->active_)
                {
                    if (((fw->mode_ & IFileWatcher::READ  ) != 0 && FD_ISSET(fw->fd_, &read_fds  ))
                     || ((fw->mode_ & IFileWatcher::WRITE ) != 0 && FD_ISSET(fw->fd_, &write_fds ))
                     || ((fw->mode_ & IFileWatcher::EXCEPT) != 0 && FD_ISSET(fw->fd_, &except_fds)))
                    {
                        auto file_watcher = fw->file_watcher_.lock();
                        if (file_watcher)
                        {
                            auto delegate = file_watcher->delegate_.lock();
                            if (delegate)
                                delegate->OnFileUpdate(*file_watcher.get(), file_watcher->delegate_data_);
                        }
                    }
                }
            }

            // Test for timer expirations, and don't erase stale elements
            now = System::CurrentTimeMS();
            for (auto ts = timers_.begin(); ts != timers_.end(); ++ts)
            {
                if (ts->active_ && ts->next_timeout_ <= now)
                {
                    // Adjust according to next timeout or cancel if non-repeating
                    ts->active_ &= ts->repeat_;

                    ts->next_timeout_ += ts->timeout_ms_;
                    if (ts->next_timeout_ <= now)
                        ts->next_timeout_ = now; // Reset due to drift

                    auto timer = ts->timer_.lock();
                    if (timer)
                    {
                        auto delegate = timer->delegate_.lock();
                        if (delegate)
                            delegate->TimerUpdate(*timer.get(), timer->delegate_data_);
                    }
                }
            }
        }
    }
    catch (...)
    {
        loop_entered_ = false;
    }
}

} // end of anonymous


std::shared_ptr<SelectMainLoop> SelectMainLoop::Create()
{
    auto self = std::make_shared<_SelectMainLoop>();
    return std::static_pointer_cast<SelectMainLoop>(self);
}

SelectMainLoop::SelectMainLoop()
{
}

SelectMainLoop::~SelectMainLoop()
{
}

