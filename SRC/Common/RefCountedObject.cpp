#include "stdafx.h"

#include "RefCountedObject.h"

namespace ECSEngine
{

RefCountedObject::RefCountedObject()
#ifdef PERFORM_SECURITY_CHECKS
    : FRefCounter(new ReferenceCounter())
#endif
{
#ifdef PERFORM_SECURITY_CHECKS
    AssertRelease(FRefCounter != nullptr);
    FRefCounter->IncrementRefCounter();
#endif
}
RefCountedObject::~RefCountedObject()
{
#ifdef PERFORM_SECURITY_CHECKS
    AssertRelease(FRefCounter != nullptr);
    if (FRefCounter->GetRefCounts() == 1)
        delete FRefCounter;
    else
        FRefCounter->DecrementRefCounter();
#endif
}

RefCountedObject::RefCountedObject(const RefCountedObject& other)
{
#ifdef PERFORM_SECURITY_CHECKS
    if (FRefCounter != nullptr)
    {
        if (FRefCounter->GetRefCounts() < 1)
            delete FRefCounter;
        else
            FRefCounter->DecrementRefCounter();
    }
    FRefCounter = other.FRefCounter;
    AssertRelease(FRefCounter != nullptr);
    FRefCounter->IncrementRefCounter();
#endif
}

RefCountedObject::RefCountedObject(RefCountedObject&& other)
{
#ifdef PERFORM_SECURITY_CHECKS
    if (FRefCounter != nullptr)
    {
        if (FRefCounter->GetRefCounts() < 1)
            delete FRefCounter;
        else
            FRefCounter->DecrementRefCounter();
    }

    FRefCounter = other.FRefCounter;
    AssertRelease(FRefCounter != nullptr);
    other.FRefCounter = nullptr;
#endif
}

void RefCountedObject::operator=(RefCountedObject&& other)
{
#ifdef PERFORM_SECURITY_CHECKS
    if (FRefCounter != nullptr)
    {
        if (FRefCounter->GetRefCounts() < 1)
            delete FRefCounter;
        else
            FRefCounter->DecrementRefCounter();
    }

    FRefCounter = other.FRefCounter;
    AssertRelease(FRefCounter != nullptr);
    other.FRefCounter = nullptr;
#endif
}

void RefCountedObject::operator=(const RefCountedObject& other)
{
#ifdef PERFORM_SECURITY_CHECKS
    if (FRefCounter != nullptr)
    {
        if (FRefCounter->GetRefCounts() < 1)
            delete FRefCounter;
        else
            FRefCounter->DecrementRefCounter();
    }
    FRefCounter = other.FRefCounter;
    AssertRelease(FRefCounter != nullptr);
    FRefCounter->IncrementRefCounter();
#endif
}

} // namespace ECSEngine
