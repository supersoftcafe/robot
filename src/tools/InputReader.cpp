
#include <cstring>
#include <iostream>

#include <fcntl.h>
#include <unistd.h>

#include "../interfaces/IFileWatcher.h"
#include "../tools/InputReader.h"
#include "../tools/System.h"



InputReaderException::InputReaderException(const char* message) : what_(message)
{
}

const char* InputReaderException::what() const throw()
{
    return what_;
}


IInputReaderDelegate::IInputReaderDelegate()
{
}

IInputReaderDelegate::~IInputReaderDelegate()
{
}

void IInputReaderDelegate::InputReaderUpdate(InputReader& reader, int data)
{
    DoInputReaderUpdate(reader, data);
}


InputReader::InputReader()
{
}

InputReader::~InputReader()
{
}

void InputReader::set_delegate(const std::shared_ptr<IInputReaderDelegate>& value)
{
    do_set_delegate(value);
}

std::shared_ptr<IInputReaderDelegate> InputReader::delegate()
{
    return do_delegate();
}

void InputReader::set_delegate_data(int value)
{
    do_set_delegate_data(value);
}

int InputReader::delegate_data()
{
    return do_delegate_data();
}

bool InputReader::ReadLine(std::string& str)
{
    return DoReadLine(str);
}



namespace { // Begin anonymous namespace

class _InputReader : public InputReader, public IFileWatcherDelegate
{
public:
    _InputReader(int fd);
    virtual ~_InputReader();

    virtual void do_set_delegate(const std::shared_ptr<IInputReaderDelegate>& value) override;
    virtual std::shared_ptr<IInputReaderDelegate> do_delegate() override;

    virtual void do_set_delegate_data(int value) override;
    virtual int do_delegate_data() override;

    virtual bool FindLine(int& length, int& length_with_eol);

    virtual bool DoReadLine(std::string& str) override;

    virtual void DoOnFileUpdate(IFileWatcher& watcher, int data) override;


    std::string                           buffer_;
    std::shared_ptr<IFileWatcher>   file_watcher_;
    std::weak_ptr<IInputReaderDelegate> delegate_;
    int                            delegate_data_;
    int                                       fd_;
    bool                     file_watcher_active_;
};

_InputReader::_InputReader(int fd) : file_watcher_(System::CreateFileWatcher()), delegate_data_(0), fd_(fd), file_watcher_active_(false)
{
    int flags = fcntl(fd_, F_GETFL, 0);
    if (flags == -1)
    {
        std::cerr << "InputReader() call to fcntl failed with " << std::strerror(errno) << std::endl;
        throw InputReaderException("InputReader() call to fcntl failed");
    }

    if (fcntl(fd, F_SETFL, flags|O_NONBLOCK) == -1)
    {
        std::cerr << "InputReader() call to fcntl failed with " << std::strerror(errno) << std::endl;
        throw InputReaderException("InputReader() call to fcntl failed");
    }
}

_InputReader::~_InputReader()
{
    file_watcher_->Stop();
}

void _InputReader::do_set_delegate(const std::shared_ptr<IInputReaderDelegate>& value)
{
    delegate_ = value;
}

std::shared_ptr<IInputReaderDelegate> _InputReader::do_delegate()
{
    return delegate_.lock();
}

void _InputReader::do_set_delegate_data(int value)
{
    delegate_data_ = value;
}

int _InputReader::do_delegate_data()
{
    return delegate_data_;
}

bool _InputReader::FindLine(int& length, int& length_with_eol)
{
    auto size = buffer_.size();
    auto data = buffer_.data();
    for (decltype(size) index = 0; index < size; ++index)
    {
        char chr = data[index];
        if (chr == '\r' || chr == '\n' || chr == '\0')
        {
            length = index;
            while (++index < size)
            {
                chr = data[index];
                if (chr != '\r' && chr != '\n' && chr == '\0')
                    break;
            }
            length_with_eol = index;
            return true;
        }
    }
    return false;
}

void _InputReader::DoOnFileUpdate(IFileWatcher& watcher, int data)
{
    const int READ_SIZE = 512;
    buffer_.resize(buffer_.size() + READ_SIZE);
    int length = read(fd_, &buffer_[buffer_.size()-READ_SIZE], READ_SIZE);
    if (length == -1)
    {
        std::cerr << "InputReader() call to read failed with " << std::strerror(errno) << std::endl;
        throw InputReaderException("InputReader() call to read failed");
    }

    if (length < READ_SIZE)
    {
        buffer_.resize(buffer_.size() - (READ_SIZE - length));
    }

    int a, b;
    if (FindLine(a, b))
    {
        // No more IO notifications until the line is read
        file_watcher_->Stop();
        file_watcher_active_ = false;

        // Notify the delegate
        std::shared_ptr<IInputReaderDelegate> delegate = delegate_.lock();
        if (delegate) delegate->InputReaderUpdate(*this, delegate_data_);
    }
}

bool _InputReader::DoReadLine(std::string& str)
{
    if (!file_watcher_active_)
    {
        // Re-start the file watcher
        file_watcher_->Start();
        file_watcher_active_ = true;
    }

    int length, length_with_eol;
    if (FindLine(length, length_with_eol))
    {
        str.assign(buffer_, 0, length);
        buffer_.erase(0, length_with_eol);
        return true;
    }
    return false;
}



} // End anonymouse namespace



std::shared_ptr<InputReader> InputReader::Create(int fd)
{
    auto self = std::make_shared<_InputReader>(fd);
    self->file_watcher_->set_delegate(self);
    self->file_watcher_->set_file_descriptor(fd);
    self->file_watcher_->Start();
    self->file_watcher_active_ = true;
    return std::static_pointer_cast<InputReader>(self);
}


