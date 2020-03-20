#pragma once
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
        static_assert(ModuleTraits<T>::GetModuleId() < sizeof(FKey) * 8, "You should consider removing modules or increasing the size of the entity module key");
        return FKey & (1 << ModuleTraits<T>::GetModuleId());
    }

    const bool HasModule(const u32 parModuleId) const
    {
        AssertRelease(parModuleId < ModulePoolSize);
        return FKey & (1 << parModuleId);
    }

    template<typename T>
    void SetHasModule()
    {
        static_assert(ModuleTraits<T>::GetModuleId() < sizeof(FKey) * 8, "You should consider removing modules or increasing the size of the entity module key");
        SetHasModule(ModuleTraits<T>::GetModuleId());
    }

    template<typename T>
    void RemoveModule()
    {
        static_assert(ModuleTraits<T>::GetModuleId() < sizeof(FKey) * 8, "You should consider removing modules or increasing the size of the entity module key");
        RemoveModule(ModuleTraits<T>::GetModuleId());
    }

    void SetHasModule(const u32 parModuleId)
    {
        AssertRelease(parModuleId < ModulePoolSize);
        FKey |= (1 << parModuleId);
    }

    void RemoveModule(const u32 parModuleId)
    {
        AssertRelease(parModuleId < ModulePoolSize);
        FKey &= ~(1 << parModuleId);
    }

    template<class Archive>
    u32 save_minimal(Archive const&) const
    {
        return FKey;
    }

    template<class Archive>
    void load_minimal(Archive const&, const u32& value)
    {
        FKey = value;
    }

private:
    u32 FKey;
};
} // namespace ECSEngine
