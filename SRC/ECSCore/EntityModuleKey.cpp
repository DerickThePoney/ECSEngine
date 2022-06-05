#include "stdafx.h"

#include "EntityModuleKey.h"

#include "ModuleId.h"

namespace ECSEngine
{
EntityModuleKey::EntityModuleKey()
    : FKey(0)
{
}

const bool EntityModuleKey::HasModule(const u32 parModuleId) const
{
    AssertRelease(parModuleId < MaxModuleNumber);
    return FKey.GetValue(parModuleId);
}

void EntityModuleKey::SetHasModule(const u32 parModuleId)
{
    AssertRelease(parModuleId < MaxModuleNumber);
    FKey.SetBit(parModuleId, true);
}

void EntityModuleKey::RemoveModule(const u32 parModuleId)
{
    AssertRelease(parModuleId < MaxModuleNumber);
    FKey.SetBit(parModuleId, false);
}

const BitSet<MaxModuleNumber>& EntityModuleKey::GetKey() const
{
    return FKey;
}

template<typename T>
const bool EntityModuleKey::HasModule() const
{
    static_assert(ModuleTraits<T>::GetModuleId() < MaxModuleNumber, "You should consider removing modules or increasing the size of the entity module key");
    return HasModule(ModuleTraits<T>::GetModuleId());
}

#define DECLARE_MODULE_AND_TEMPLATE(NAME, TEMPLATE) template const bool EntityModuleKey::HasModule<NAME>() const;
#include "ModuleList.inl"
#undef DECLARE_MODULE_AND_TEMPLATE

template<typename T>
void EntityModuleKey::SetHasModule()
{
    static_assert(ModuleTraits<T>::GetModuleId() < MaxModuleNumber, "You should consider removing modules or increasing the size of the entity module key");
    SetHasModule(ModuleTraits<T>::GetModuleId());
}

#define DECLARE_MODULE_AND_TEMPLATE(NAME, TEMPLATE) template void EntityModuleKey::SetHasModule<NAME>();
#include "ModuleList.inl"
#undef DECLARE_MODULE_AND_TEMPLATE

template<typename T>
void EntityModuleKey::RemoveModule()
{
    static_assert(ModuleTraits<T>::GetModuleId() < MaxModuleNumber, "You should consider removing modules or increasing the size of the entity module key");
    RemoveModule(ModuleTraits<T>::GetModuleId());
}

#define DECLARE_MODULE_AND_TEMPLATE(NAME, TEMPLATE) template void EntityModuleKey::RemoveModule<NAME>();
#include "ModuleList.inl"
#undef DECLARE_MODULE_AND_TEMPLATE

} // namespace ECSEngine
