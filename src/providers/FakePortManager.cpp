
#include <map>
#include <vector>
#include <cstring>
#include <iostream>
#include "../providers/FakePortManager.h"


namespace { // Begin anonymous namespace


FakePortDescriptor fake_ports[] = {
        {"gpio/7" , true, false, 0.0f},
        {"gpio/8" , true, false, 0.0f},
        {"gpio/9" , true, false, 0.0f},
        {"gpio/10", true, false, 0.0f}
};


class _FakePortManager;
class _FakePort : public IPort
{
public:
    _FakePort(const FakePortDescriptor& pd);
    virtual ~_FakePort();

private:
    friend class _FakePortManager;

    std::string                      name_;
    FakePortDescriptor                 pd_;
    std::weak_ptr<IPortDelegate> delegate_;
    int                     delegate_data_;

    virtual const char* do_name() override;

    virtual void do_set_delegate(const std::shared_ptr<IPortDelegate>& value) override;
    virtual std::shared_ptr<IPortDelegate> do_delegate() override;

    virtual void  do_set_delegate_data(int data) override;
    virtual int do_delegate_data() override;

    virtual void do_set_writable(bool value) override;
    virtual bool do_writable() override;

    virtual void do_set_binary_level(bool value) override;
    virtual bool do_binary_level() override;

    virtual void do_set_analogue_level(float value) override;
    virtual float do_analogue_level() override;
};


class _FakePortManager : public FakePortManager
{
public:
    _FakePortManager(const FakePortDescriptor* static_ports_array, int port_count);
    virtual ~_FakePortManager();

private:
    std::vector<std::shared_ptr<_FakePort> > ports_;

    virtual std::shared_ptr<IPort> DoFindPort(const char* name) override;
};



_FakePort::_FakePort(const FakePortDescriptor& pd) : name_(pd.name), pd_(pd), delegate_data_(0)
{
    pd_.name = name_.c_str();
}

_FakePort::~_FakePort()
{
}

const char* _FakePort::do_name()
{
    return pd_.name;
}

void _FakePort::do_set_delegate(const std::shared_ptr<IPortDelegate>& value)
{
    delegate_ = value;
}
std::shared_ptr<IPortDelegate> _FakePort::do_delegate()
{
    return delegate_.lock();
}

void  _FakePort::do_set_delegate_data(int value)
{
     delegate_data_ = value;
}
int _FakePort::do_delegate_data()
{
    return delegate_data_;
}

void _FakePort::do_set_writable(bool value)
{
    pd_.writable = value;
}
bool _FakePort::do_writable()
{
    return pd_.writable;
}

void _FakePort::do_set_binary_level(bool value)
{
    pd_.binary_level = value;
    pd_.analogue_level = value ? 1.0f : 0.0f;
}
bool _FakePort::do_binary_level()
{
    return pd_.binary_level;
}

void _FakePort::do_set_analogue_level(float value)
{
    pd_.binary_level = value>0.5f;
    pd_.analogue_level = value;
}
float _FakePort::do_analogue_level()
{
    return pd_.analogue_level;
}



_FakePortManager::_FakePortManager(const FakePortDescriptor* static_ports_array, int port_count)
{
    for (int index = 0; index < port_count; ++index)
        ports_.push_back(std::make_shared<_FakePort>(static_ports_array[index]));
}

_FakePortManager::~_FakePortManager()
{
}

std::shared_ptr<IPort> _FakePortManager::DoFindPort(const char* name)
{
    for (std::shared_ptr<_FakePort>& port : ports_)
        if (port->name_ == name)
            return std::static_pointer_cast<IPort>(port);
    return std::shared_ptr<IPort>();
}

} // End anonymous namespace



FakePortManager::FakePortManager()
{
}

FakePortManager::~FakePortManager()
{
}

std::shared_ptr<FakePortManager> FakePortManager::Create(const FakePortDescriptor* ports, int port_count)
{
    if (!ports)
    {
        std::cerr << "Null argument named 'ports' when creating FakePortManager" << std::endl;
        throw PortManagerException("Null argument named 'port_count' when creating FakePortManager");
    }
    if (port_count < 0)
    {
        std::cerr << "Out of range argument named 'port_count' when creating FakePortManager" << std::endl;
        throw PortManagerException("Out of range argument named 'port_count' when creating FakePortManager");
    }
    return std::static_pointer_cast<FakePortManager>(std::make_shared<_FakePortManager>(ports, port_count));
}

std::shared_ptr<FakePortManager> FakePortManager::Create()
{
    return Create(fake_ports, sizeof(fake_ports)/sizeof(*fake_ports));
}


