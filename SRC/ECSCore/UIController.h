#pragma once
#include "ModuleAccessor.h"
#include "WorldIds.h"

#include <brigand/brigand.hpp>

namespace ECSEngine
{
namespace UI
{
template<typename T, Worlds::Type _World>
struct MC
{
    using ModuleType = std::decay_t<T>;

    constexpr static Worlds::Type World = _World;
    using Accessor = ManualLockModuleAccessor<ModuleType>;

    static Accessor Construct() { return Accessor(_World); }
};

template<typename... MCs>
class UIController
{

protected:
    using AccessorList = brigand::list<typename MCs::Accessor...>;
    using AccessorMap = brigand::map<brigand::pair<typename MCs::ModuleType, typename MCs::Accessor>...>;

    template<typename T>
    using GetAccessor = brigand::lookup<AccessorMap, T>;

    using TupleType = brigand::as_tuple<AccessorList>;

public:
    UIController()
        : FAccessors(std::move(typename MCs::Accessor(MCs::World))...)
    {
    }

    UIController(const UIController&) = delete;
    UIController(UIController&&) = delete;
    UIController& operator=(const UIController&) = delete;
    UIController& operator=(UIController&&) = delete;

    virtual ~UIController() { }

    void Init()
    {
#ifdef PERFORM_SECURITY_CHECKS
        FVirtualInitCalled = false;
#endif
        VirtualInit();
        AlwaysCheckedAssert(FVirtualInitCalled);
    }

    void Update()
    {
#ifdef PERFORM_SECURITY_CHECKS
        FVirtualUpdateCalled = false;
#endif
        LockDependencies();
        VirtualUpdate();
        UnlockDependencies();
        AlwaysCheckedAssert(FVirtualUpdateCalled);
    }

    void Destroy()
    {
#ifdef PERFORM_SECURITY_CHECKS
        FVirtualDestroyCalled = false;
#endif
        VirtualDestroy();
        AlwaysCheckedAssert(FVirtualDestroyCalled);
    }

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
    virtual void VirtualInit()
    {
#ifdef PERFORM_SECURITY_CHECKS
        FVirtualInitCalled = true;
#endif
    }
    virtual void VirtualUpdate()
    {
#ifdef PERFORM_SECURITY_CHECKS
        FVirtualUpdateCalled = true;
#endif
    }
    virtual void VirtualDestroy()
    {
#ifdef PERFORM_SECURITY_CHECKS
        FVirtualDestroyCalled = true;
#endif
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

#ifdef PERFORM_SECURITY_CHECKS
    bool FVirtualInitCalled;
    bool FVirtualUpdateCalled;
    bool FVirtualDestroyCalled;
#endif
};
} // namespace UI
} // namespace ECSEngine