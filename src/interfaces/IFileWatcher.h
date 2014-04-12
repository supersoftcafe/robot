
#ifndef __IFILEWATCHER_H
#define __IFILEWATCHER_H

#include <memory>
#include <exception>

#include "../interfaces/IGenericService.h"


class FileWatcherException : public std::exception
{
public:
    FileWatcherException(const char* message);

    virtual const char* what() const throw();

private:
    const char* what_;
};


class IFileWatcher;


class IFileWatcherFactory
{
public:
    std::shared_ptr<IFileWatcher> CreateFileWatcher();

protected:
    IFileWatcherFactory();
    virtual ~IFileWatcherFactory();

private:
    virtual std::shared_ptr<IFileWatcher> DoCreateFileWatcher() = 0;
};


class IFileWatcherDelegate
{
public:
    void OnFileUpdate(IFileWatcher& watcher, int data);

protected:
    IFileWatcherDelegate();
    virtual ~IFileWatcherDelegate();

private:
    virtual void DoOnFileUpdate(IFileWatcher& watcher, int data) = 0;
};


class IFileWatcher : public IGenericService
{
public:
    enum {READ = 0x1, WRITE = 0x2, EXCEPT = 0x4};

    void set_delegate(const std::shared_ptr<IFileWatcherDelegate>& value);
    std::shared_ptr<IFileWatcherDelegate> delegate();

    void set_delegate_data(int value);
    int delegate_data();

    void set_file_descriptor(int value);
    int file_descriptor();

    void set_mode(int value);
    int mode();

protected:
    IFileWatcher();
    virtual ~IFileWatcher();

private:
    virtual void do_set_delegate(const std::shared_ptr<IFileWatcherDelegate>& value) = 0;
    virtual std::shared_ptr<IFileWatcherDelegate> do_delegate() = 0;

    virtual void do_set_delegate_data(int value) = 0;
    virtual int do_delegate_data() = 0;

    virtual void do_set_file_descriptor(int value) = 0;
    virtual int do_file_descriptor() = 0;

    virtual void do_set_mode(int value) = 0;
    virtual int do_mode() = 0;
};

#endif // __IFILEWATCHER_H

