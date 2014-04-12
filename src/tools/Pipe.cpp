
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

#include "../tools/Pipe.h"


PipeException::PipeException(const char* message) : what_(message)
{
}

const char* PipeException::what() const throw()
{
    return what_;
}

static void make_async(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1)
    {
        std::cerr << "F_GETFL failed with error " << std::strerror(errno) << std::endl;
        throw PipeException("F_GETFL failed");
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        std::cerr << "F_SETFL failed with error " << std::strerror(errno) << std::endl;
        throw PipeException("F_SETFL failed");
    }
}

Pipe::Pipe(bool async0, bool async1)
{
    if (pipe(fd) == -1)
    {
        std::cerr << "Pipe creation failed with error " << std::strerror(errno) << std::endl;
        throw PipeException("Pipe creation failed");
    }

    try
    {
        if (async0) make_async(fd[0]);
        if (async1) make_async(fd[1]);
    }
    catch (...)
    {
        close(fd[0]);
        close(fd[1]);
        throw;
    }
}

Pipe::~Pipe()
{
    close(fd[0]);
    close(fd[1]);
}


