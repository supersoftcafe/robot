
#include <iostream>

#include "../interfaces/IFileWatcher.h"


FileWatcherException::FileWatcherException(const char* message) : what_(message)
{
}

const char* FileWatcherException::what() const throw()
{
    return what_;
}



IFileWatcherFactory::IFileWatcherFactory()
{
}

IFileWatcherFactory::~IFileWatcherFactory()
{
}

std::shared_ptr<IFileWatcher> IFileWatcherFactory::CreateFileWatcher()
{
    return DoCreateFileWatcher();
}


IFileWatcherDelegate::IFileWatcherDelegate()
{
}

IFileWatcherDelegate::~IFileWatcherDelegate()
{
}

void IFileWatcherDelegate::OnFileUpdate(IFileWatcher& watcher, int data)
{
    DoOnFileUpdate(watcher, data);
}


IFileWatcher::IFileWatcher()
{
}

IFileWatcher::~IFileWatcher()
{
}

void IFileWatcher::set_delegate(const std::shared_ptr<IFileWatcherDelegate>& value)
{
    do_set_delegate(value);
}

std::shared_ptr<IFileWatcherDelegate> IFileWatcher::delegate()
{
    return do_delegate();
}

void IFileWatcher::set_delegate_data(int value)
{
    do_set_delegate_data(value);
}

int IFileWatcher::delegate_data()
{
    return do_delegate_data();
}

void IFileWatcher::set_file_descriptor(int value)
{
    do_set_file_descriptor(value);
}

int IFileWatcher::file_descriptor()
{
    return file_descriptor();
}

void IFileWatcher::set_mode(int value)
{
    if ((value & ~(READ|WRITE|EXCEPT)) != 0 || value == 0)
    {
        std::cerr << "IFileWatcher::Start called with invalid or empty mode" << std::endl;
        throw FileWatcherException("IFileWatcher::Start called with invalid or empty mode");
    }
    do_set_mode(value);
}

int IFileWatcher::mode()
{
    return do_mode();
}

/*
    if (fd < 0)
    {
        std::cerr << "IFileWatcher::Start called with invalid fd" << std::endl;
        throw FileWatcherException("IFileWatcher::Start called with invalid fd");
    }
    if ((mode & ~(READ|WRITE|EXCEPT)) != 0)
    {
        std::cerr << "IFileWatcher::Start called with invalid mode" << std::endl;
        throw FileWatcherException("IFileWatcher::Start called with invalid mode");
    }
*/

