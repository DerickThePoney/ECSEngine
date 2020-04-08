#pragma once
#include "ReferenceCounter.h"

namespace ECSEngine
{

class RefCountedObject
{
public:
    RefCountedObject();
    virtual ~RefCountedObject();

    RefCountedObject(const RefCountedObject& other);
    RefCountedObject(RefCountedObject&& other);

    void operator=(const RefCountedObject& other);
    void operator=(RefCountedObject&& other);

#ifdef PERFORM_SECURITY_CHECKS
    const ReferenceCounter* const GetReferenceCounter() const { return FRefCounter; }

private:
    ReferenceCounter* FRefCounter = nullptr;
#endif
};

} // namespace ECSEngine
