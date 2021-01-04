#pragma once
#include "PoolAllocator.h"

namespace ECSEngine
{
class GenericMessage
{
public:
    GenericMessage(u32 parId, void* parMessageDataToStealOwnershipOf);
    virtual ~GenericMessage() { }

    template<typename T>
    const T* UserDataAs() const
    {
        const void* userData = UserData();
        if (userData != nullptr)
        {
            const T* result = reinterpret_cast<const T*>(userData);
            AssertRelease(result != nullptr);
            return result;
        }
        return nullptr;
    }

    u32 Id() const { return FId; }
    const void* UserData() const { return FUserData; }

    template<typename T>
    T* UserDataAs()
    {
        void* userData = UserData();
        if (userData != nullptr)
        {
            T* result = reinterpret_cast<T*>(userData);
            AssertRelease(result != nullptr);
            return result;
        }
        return nullptr;
    }

protected:
    void* UserData() { return FUserData; }

private:
    u32 FId;
    void* FUserData;
};

template<typename T>
class GenericMessageImplem : public GenericMessage
{
    TEMPLATE_POOL_ALLOCATED(GenericMessageImplem, T);

public:
    GenericMessageImplem(u32 parId, void* parMessageDataToStealOwnershipOf)
        : GenericMessage(parId, parMessageDataToStealOwnershipOf)
    {
    }
    virtual ~GenericMessageImplem() { delete UserDataAs<T>(); }
};

} // namespace ECSEngine