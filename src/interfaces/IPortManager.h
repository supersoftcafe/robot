
#ifndef __IPORTMANAGER_H_
#define __IPORTMANAGER_H_

#include <memory>
#include "../tools/Exception.h"

class PortManagerException : public Exception
{
public:
    PortManagerException(const char* message);
};

class IPort;
class IPortManager
{
public:
    std::shared_ptr<IPort> FindPort(const char* name);

protected:
    IPortManager();
    virtual ~IPortManager();

private:
    virtual std::shared_ptr<IPort> DoFindPort(const char* name) = 0;
};

#endif /* __IPORTMANAGER_H_ */
