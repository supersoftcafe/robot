
#include "../interfaces/IPortManager.h"


PortManagerException::PortManagerException(const char* message) : Exception(message)
{
}


IPortManager::IPortManager()
{

}
IPortManager::~IPortManager()
{

}

std::shared_ptr<IPort> IPortManager::FindPort(const char* name)
{
    return DoFindPort(name);
}
