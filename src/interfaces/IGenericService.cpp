
#include "../interfaces/IGenericService.h"

IGenericService::IGenericService()
{
}

IGenericService::~IGenericService()
{
}

void IGenericService::Start()
{
    DoStart();
}

void IGenericService::Stop()
{
    DoStop();
}


