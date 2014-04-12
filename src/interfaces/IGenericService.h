
#ifndef __IGENERICSERVICE_H
#define __IGENERICSERVICE_H

#include <memory>

class IGenericService
{
public:
    void Start();
    void Stop();

protected:
    IGenericService();
    virtual ~IGenericService();

private:
    virtual void DoStart() = 0;
    virtual void DoStop() = 0;

};

#endif // __IGENERICSERVICE_H

