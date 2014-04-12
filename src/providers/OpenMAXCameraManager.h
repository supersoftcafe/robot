
#ifndef __OPENMAXCAMERAMANAGER_H
#define __OPENMAXCAMERAMANAGER_H

#include "../interfaces/ICameraManager.h"

class OpenMAXCameraManager : public ICameraManager
{
public:
    static std::shared_ptr<OpenMAXCameraManager> Create();

    virtual void test() = 0;

protected:
    OpenMAXCameraManager();
    virtual ~OpenMAXCameraManager();
};

#endif // __OPENMAXCAMERAMANAGER_H

