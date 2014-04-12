
#ifndef __CAMERAMANAGER_H
#define __CAMERAMANAGER_H

#include <memory>
#include "../tools/Exception.h"

class CameraManagerException : public Exception
{
public:
    CameraManagerException(const char* message);
};

class ICamera;
class ICameraManager
{
public:
    std::shared_ptr<ICamera> FindCamera(const char* name);

protected:
    ICameraManager();
    virtual ~ICameraManager();

private:
    virtual std::shared_ptr<ICamera> DoFindCamera(const char* name) = 0;
};

#endif // __CAMERAMANAGER_H
