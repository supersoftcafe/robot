
#ifndef __PIPE_H
#define __PIPE_H

#include <exception>

class PipeException : public std::exception
{
public:
    PipeException(const char* message);

    virtual const char* what() const throw();

private:
    const char* what_;
};

class Pipe
{
public:
    Pipe(bool async0 = true, bool async1 = false);
    ~Pipe();

    int fd0() {return fd[0]; }
    int fd1() {return fd[1]; }

private:
    int fd[2];
};

#endif // __PIPE_H

