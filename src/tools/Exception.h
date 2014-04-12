
#ifndef __EXCEPTION_H
#define __EXCEPTION_H

#include <exception>

class Exception : public std::exception
{
public:
    Exception(const char* message);

    virtual const char* what() const throw();

private:
    const char* what_;
};


#endif // __EXCEPTION_H

