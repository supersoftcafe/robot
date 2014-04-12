
#include "../tools/Exception.h"

Exception::Exception(const char* message) : what_(message)
{
}

const char* Exception::what() const throw()
{
    return what_;
}

