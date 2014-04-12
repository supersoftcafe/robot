
#include "../interfaces/ICameraManager.h"

CameraManagerException::CameraManagerException(const char* message) : Exception(message)
{
}

ICameraManager::ICameraManager()
{
}

ICameraManager::~ICameraManager()
{
}


std::shared_ptr<ICamera> ICameraManager::FindCamera(const char* name)
{
    return DoFindCamera(name);
}


