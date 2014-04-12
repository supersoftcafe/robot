
#ifndef __IPORT_H_
#define __IPORT_H_

#include <memory>
#include <exception>

class PortException : public std::exception
{
public:
    PortException(const char* message);

    virtual const char* what() const throw();

private:
    const char* what_;
};


class IPort;
class IPortDelegate
{
public:
    void OnPortUpdate(IPort& port, int data);

protected:
    IPortDelegate();
    virtual ~IPortDelegate();

private:
    virtual void DoOnPortUpdate(IPort& port, int data) = 0;
};


class IPort
{
public:
    const char* name();

    void set_delegate(const std::shared_ptr<IPortDelegate>& value);
    std::shared_ptr<IPortDelegate> delegate();

    void set_delegate_data(int data);
    int delegate_data();

    void set_writable(bool value);
    bool writable();

    void set_binary_level(bool value);
    bool binary_level();

    void set_analogue_level(float value);
    float analogue_level();

protected:
    IPort();
    virtual ~IPort();

private:
    virtual const char* do_name() = 0;

    virtual void do_set_delegate(const std::shared_ptr<IPortDelegate>& value) = 0;
    virtual std::shared_ptr<IPortDelegate> do_delegate() = 0;

    virtual void  do_set_delegate_data(int value) = 0;
    virtual int do_delegate_data() = 0;

    virtual void do_set_writable(bool value) = 0;
    virtual bool do_writable() = 0;

    virtual void do_set_binary_level(bool value) = 0;
    virtual bool do_binary_level() = 0;

    virtual void do_set_analogue_level(float value) = 0;
    virtual float do_analogue_level() = 0;
};


#endif /* __IPORT_H_ */

