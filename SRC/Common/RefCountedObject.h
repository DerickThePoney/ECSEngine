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

    ReferenceCounter* const GetReferenceCounterForWriting() const { return FRefCounter; }
    const ReferenceCounter* const GetReferenceCounter() const { return FRefCounter; }

private:
    ReferenceCounter* FRefCounter = nullptr;
};

} // namespace ECSEngine
