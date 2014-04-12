
#ifndef __FAKEPORTMANAGER_H_
#define __FAKEPORTMANAGER_H_

#include <memory>
#include "../interfaces/IPort.h"
#include "../interfaces/IPortManager.h"

struct FakePortDescriptor
{
    const char*     name;
    bool        writable;
    bool    binary_level;
    float analogue_level;
};

class FakePortManager : public IPortManager
{
public:
    static std::shared_ptr<FakePortManager> Create();
    static std::shared_ptr<FakePortManager> Create(const FakePortDescriptor* static_ports_array, int port_count);

protected:
    FakePortManager();
    virtual ~FakePortManager();
};

#endif /* __FAKEPORTMANAGER_H_ */
