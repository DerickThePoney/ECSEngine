#include "stdafx.h"

#include "RefCountedObject.h"

namespace ECSEngine
{

RefCountedObject::RefCountedObject()
    : FRefCounter(new ReferenceCounter())
{
    AssertRelease(FRefCounter != nullptr);
    FRefCounter->IncrementRefCounter();
}
RefCountedObject::~RefCountedObject()
{
    AssertRelease(FRefCounter != nullptr);
    if (FRefCounter->GetRefCounts() == 1)
    {
        delete FRefCounter;
        FRefCounter = nullptr;
    }
    else
        FRefCounter->DecrementRefCounter();
}

RefCountedObject::RefCountedObject(const RefCountedObject& other)
{
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
}

RefCountedObject::RefCountedObject(RefCountedObject&& other)
{
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
}

void RefCountedObject::operator=(RefCountedObject&& other)
{
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
}

void RefCountedObject::operator=(const RefCountedObject& other)
{
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
}

} // namespace ECSEngine
