#pragma once

#include "ModuleAccessor.h"
#include "WorldIds_fwd.h"
#include "brigand/adapted/tuple.hpp"
#include "brigand/sequences/has_key.hpp"
#include "brigand/sequences/list.hpp"
#include "brigand/sequences/map.hpp"

namespace ECSEngine
{
template<typename T, EEntityWorlds _World>
struct MC
{
    using ModuleType = std::decay_t<T>;

    constexpr static EEntityWorlds World = _World;
    using Accessor = ManualLockModuleAccessor<ModuleType>;

    static Accessor Construct() { return Accessor(_World); }
};

template<typename... MCs>
class ScopedModuleAccessor
{
protected:
    using AccessorList = brigand::list<typename MCs::Accessor...>;
    using AccessorMap = brigand::map<brigand::pair<typename MCs::ModuleType, typename MCs::Accessor>...>;

    template<typename T>
    using GetAccessor = brigand::lookup<AccessorMap, T>;

    using TupleType = brigand::as_tuple<AccessorList>;

public:
    ScopedModuleAccessor()
        : FAccessors(std::move(typename MCs::Accessor(MCs::World))...)
    {
        LockDependencies();
    }

    ~ScopedModuleAccessor() { UnlockDependencies(); }

    ScopedModuleAccessor(const ScopedModuleAccessor&) = delete;
    ScopedModuleAccessor(ScopedModuleAccessor&&) = delete;
    ScopedModuleAccessor& operator=(const ScopedModuleAccessor&) = delete;
    ScopedModuleAccessor& operator=(ScopedModuleAccessor&&) = delete;

    template<typename T>
    auto& Accessor() const
    {
        using ModuleType = std::decay_t<T>;
        static_assert(brigand::has_key<AccessorMap, ModuleType>::value, "No accessor has been set for this module type");

        using AccessorType = GetAccessor<T>;
        return std::get<AccessorType>(FAccessors);
    }

    template<typename T>
    decltype(auto) GetModule(const EntityId& parId) const
    {
        return Accessor<T>()[parId];
    }

    template<typename T>
    auto& Accessor()
    {
        using ModuleType = std::decay_t<T>;
        static_assert(brigand::has_key<AccessorMap, ModuleType>::value, "No accessor has been set for this module type");

        using AccessorType = GetAccessor<T>;
        return std::get<AccessorType>(FAccessors);
    }

    template<typename T>
    decltype(auto) GetModule(const EntityId& parId)
    {
        return Accessor<T>()[parId];
    }

protected:
    void LockDependencies()
    {
        std::apply([](auto&&... args) { ((args.LockIFN()), ...); }, FAccessors);
    }
    void UnlockDependencies()
    {
        std::apply([](auto&&... args) { ((args.UnlockIFN()), ...); }, FAccessors);
    }

protected:
    TupleType FAccessors;
};

} // namespace ECSEngine