#pragma once
#include "Common/BitSet.h"
#include "Common/Constants.h"
#include "ModuleId.h"

namespace ECSEngine
{
struct EntityModuleKey
{
public:
    EntityModuleKey()
        : FKey(0)
    {
    }

    template<typename T>
    const bool HasModule() const
    {
        static_assert(ModuleTraits<T>::GetModuleId() < MaxModuleNumber, "You should consider removing modules or increasing the size of the entity module key");
        return HasModule(ModuleTraits<T>::GetModuleId());
    }

    const bool HasModule(const u32 parModuleId) const
    {
        AssertRelease(parModuleId < MaxModuleNumber);
        return FKey.GetValue(parModuleId);
    }

    template<typename T>
    void SetHasModule()
    {
        static_assert(ModuleTraits<T>::GetModuleId() < MaxModuleNumber, "You should consider removing modules or increasing the size of the entity module key");
        SetHasModule(ModuleTraits<T>::GetModuleId());
    }

    template<typename T>
    void RemoveModule()
    {
        static_assert(ModuleTraits<T>::GetModuleId() < MaxModuleNumber, "You should consider removing modules or increasing the size of the entity module key");
        RemoveModule(ModuleTraits<T>::GetModuleId());
    }

    void SetHasModule(const u32 parModuleId)
    {
        AssertRelease(parModuleId < MaxModuleNumber);
        FKey.SetBit(parModuleId, true);
    }

    void RemoveModule(const u32 parModuleId)
    {
        AssertRelease(parModuleId < MaxModuleNumber);
        FKey.SetBit(parModuleId, false);
    }

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(FKey);
    }

    const BitSet<MaxModuleNumber>& GetKey() const { return FKey; }

private:
    BitSet<MaxModuleNumber> FKey;
};
} // namespace ECSEngine
