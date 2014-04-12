
#include <iostream>
#include "../interfaces/IPort.h"


PortException::PortException(const char* message) : what_(message)
{
}

const char* PortException::what() const throw()
{
    return what_;
}


IPortDelegate::IPortDelegate()
{
}

IPortDelegate::~IPortDelegate()
{
}

void IPortDelegate::OnPortUpdate(IPort& port, int data)
{
    DoOnPortUpdate(port, data);
}


IPort::IPort()
{
}

IPort::~IPort()
{
}

const char* IPort::name()
{
    return do_name();
}

void IPort::set_delegate(const std::shared_ptr<IPortDelegate>& value)
{
    do_set_delegate(value);
}

std::shared_ptr<IPortDelegate> IPort::delegate()
{
    return do_delegate();
}

void IPort::set_delegate_data(int value)
{
    do_set_delegate_data(value);
}

int IPort::delegate_data()
{
    return do_delegate_data();
}

void IPort::set_writable(bool value)
{
    if (writable() == value)
    {
        return;
    }
    return do_set_writable(value);
}
bool IPort::writable()
{
    return do_writable();
}

void IPort::set_binary_level(bool value)
{
//    std::cerr << "Setting property named 'binary_level' on port " << name() << " to value " << value << std::endl;
    if (!writable())
    {
        std::cerr << "Attempt to set read-only property named 'binary_level' on port" << name() << std::endl;
        throw PortException("Property named 'binary_level' is read-only");
    }
    return do_set_binary_level(value);
}
bool IPort::binary_level()
{
    return do_binary_level();
}

void IPort::set_analogue_level(float value)
{
//    std::cerr << "Setting property named 'analogue_level' on port " << name() << " to value " << value << std::endl;
    if (!writable())
    {
        std::cerr << "Attempt to set read-only property named 'analogue_level' on port" << name() << std::endl;
        throw PortException("Property named 'analogue_level' is read-only");
    }
    if (value < 0.0f || value > 1.0f)
    {
        std::cerr << "Attempt to set out-of-range value on property named 'analogue_level' on port" << name() << std::endl;
        throw PortException("Property named 'analogue_level' received out-of-range value");
    }
    return do_set_analogue_level(value);
}
float IPort::analogue_level()
{
    return do_analogue_level();
}

