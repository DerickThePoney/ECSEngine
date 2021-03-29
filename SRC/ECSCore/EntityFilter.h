#pragma once
#include "Common/BitSet.h"
#include "EntityModuleKey.h"
#include "ModuleId.h"
#include "standalone/brigand.hpp"

namespace ECSEngine
{
template<typename... ModuleIds>
class EntityFilter
{
    using Modules = brigand::list<ModuleIds...>;

    struct AddModuleId
    {
        template<typename U>
        void operator()(brigand::type_<U>)
        {
            FBitSet.SetBit(ModuleTraits<U>::GetModuleId(), true);
        }

        BitSet<MaxModuleNumber>& FBitSet;
    };

public:
    EntityFilter() { brigand::for_each<Modules>(AddModuleId{ FFilter }); }

    template<typename Module>
    bool HasModule()
    {
        return FFilter.GetValue(ModuleTraits<Module>::GetModuleId());
    }

    bool IsSubsetOf(const EntityModuleKey& parEntityModulesKey) { FFilter.IsSubSetOf(parEntityModulesKey.GetKey()); }

private:
    BitSet<MaxModuleNumber> FFilter;
};
} // namespace ECSEngine
