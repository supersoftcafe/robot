
#include <queue>
#include <mutex>
#include <cstring>
#include <iostream>

#include <unistd.h>
#include <sys/eventfd.h>

#include "../interfaces/IFileWatcher.h"
#include "../providers/PollEventQueue.h"
#include "../tools/System.h"


namespace { // Start anonymous namespace

class _PollEventQueue : public PollEventQueue, public IFileWatcherDelegate
{
public:
    _PollEventQueue();
    virtual ~_PollEventQueue();

    virtual void DoOnFileUpdate(IFileWatcher& watcher, int data) override;
    virtual void DoPostEvent(const std::shared_ptr<IEvent>&) override;
    virtual void DoPostEvent(std::shared_ptr<IEvent>&&) override;

    void CheckedWrite();

    std::shared_ptr<IFileWatcher> file_watcher_;
    std::queue<std::shared_ptr<IEvent>>  queue_;
    std::mutex                           mutex_;
    int                               event_fd_;
};

_PollEventQueue::_PollEventQueue() : file_watcher_(System::CreateFileWatcher()), event_fd_(-1)
{
    event_fd_ = eventfd(0, EFD_NONBLOCK | EFD_SEMAPHORE);
    if (event_fd_ == -1)
    {
        std::cerr << "PollEventQueue() call to eventfd failed with error " << std::strerror(errno) << std::endl;
        throw EventQueueException("PollEventQueue() call to eventfd failed");
    }
}

_PollEventQueue::~_PollEventQueue()
{
    close(event_fd_);
}

void _PollEventQueue::DoOnFileUpdate(IFileWatcher& watcher, int data)
{
    eventfd_t events;
    if (eventfd_read(event_fd_, &events) == -1)
    {
        std::cerr << "PollEventQueue() call to eventfd_read failed with error " << std::strerror(errno) << std::endl;
        throw EventQueueException("PollEventQueue() call to eventfd_read failed");
    }

    for (; events > 0; --events)
    {
        std::shared_ptr<IEvent> event;

        // Try to fetch end remove front item
        {
            std::lock_guard<std::mutex> lock(mutex_);
            event.swap(queue_.front());
            queue_.pop();
        }

        // Call method on item now we are out of lock region
        event->Run();
    }
}

void _PollEventQueue::DoPostEvent(const std::shared_ptr<IEvent>& event)
{
    std::shared_ptr<IEvent> copy = event;
    DoPostEvent(std::move(copy));
}

void _PollEventQueue::DoPostEvent(std::shared_ptr<IEvent>&& event)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(event);
    }
    if (eventfd_write(event_fd_, 1) == -1)
    {
        std::cerr << "PollEventQueue call to eventfd_write failed with " << std::strerror(errno) << std::endl;
        throw EventQueueException("PollEventQueue call to eventfd_write failed");
    }
}

} // End anonymous namespace





std::shared_ptr<PollEventQueue> PollEventQueue::Create()
{
    auto self = std::make_shared<_PollEventQueue>();
    self->file_watcher_->set_delegate(self);
    self->file_watcher_->set_file_descriptor(self->event_fd_);
    self->file_watcher_->Start();
    return std::static_pointer_cast<PollEventQueue>(self);
}

PollEventQueue::PollEventQueue()
{
}

PollEventQueue::~PollEventQueue()
{
}


