
#ifndef __GPIOPORTMANAGER_H_
#define __GPIOPORTMANAGER_H_

#include <memory>
#include "../interfaces/IPortManager.h"

class IMainLoop;
class GPIOPortManager : public IPortManager
{
public:
    static std::shared_ptr<GPIOPortManager> Create();

protected:
    GPIOPortManager();
    ~GPIOPortManager();
};

#endif /* __GPIOPORTMANAGER_H_ */
