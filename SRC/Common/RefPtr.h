#pragma once
#include "RefCountedObject.h"

namespace ECSEngine
{

void AddReference(RefCountedObject* parRefCountedObject)
{
    if (parRefCountedObject != nullptr)
    {
        ReferenceCounter* const counter = parRefCountedObject->GetReferenceCounterForWriting();
        AssertRelease(counter != nullptr);
        counter->IncrementRefCounter();
    }
}

i32 RemoveReference(RefCountedObject* parRefCountedObject)
{
    if (parRefCountedObject != nullptr)
    {
        ReferenceCounter* counter = parRefCountedObject->GetReferenceCounterForWriting();
        AssertRelease(counter != nullptr);
        i32 refNumber = counter->DecrementRefCounter();
        AssertRelease(refNumber >= 0);
        return refNumber;
    }
    return -1;
}

template<class T>
class RefPtr
{
public:
    RefPtr()
        : FPtr(nullptr)
    {
    }

    RefPtr(const RefPtr& other)
    {
        FPtr = other.FPtr;
        AddReference(FPtr);
    }

    void operator=(const RefPtr& parOther)
    {
        FPtr = parOther.FPtr;
        AddReference(FPtr);
    }

    RefPtr(RefPtr&& other)
    {
        FPtr = other.FPtr;
        other.FPtr = nullptr;
    }

    void operator=(RefPtr&& other)
    {
        FPtr = other.FPtr;
        other.FPtr = nullptr;
    }

    ~RefPtr()
    {
        const i32 refNumber = RemoveReference(FPtr);
        if (refNumber == 0)
        {
            delete FPtr;
            FPtr = nullptr;
        }
    }

    bool operator==(const RefPtr& parOther) { return FPtr == parOther.FPtr; }
    bool operator==(const T* parOther) { return FPtr == parOther; }

    T* get() { return FPtr; }
    const T* get() const { return FPtr; }

    void reset(T* parNewPtr) { FPtr = parNewPtr; }

    T* operator->() { return get(); }
    T& operator*() { return *get(); }

private:
    T* FPtr;
};
} // namespace ECSEngine
