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

class UIController
{
public:
    virtual ~UIController() { }

    void Init()
    {
#ifdef PERFORM_SECURITY_CHECKS
        FVirtualInitCalled = false;
#endif
        VirtualInit();
        AlwaysCheckedAssert(FVirtualInitCalled);
    }

    virtual void Update()
    {
        if (!FShow)
            return;

#ifdef PERFORM_SECURITY_CHECKS
        FVirtualUpdateCalled = false;
#endif
        VirtualUpdate();
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

    void Show(bool parShow) { FShow = parShow; }
    bool Shown() const { return FShow; }

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
    bool FShow = false;

#ifdef PERFORM_SECURITY_CHECKS
    bool FVirtualInitCalled;
    bool FVirtualUpdateCalled;
    bool FVirtualDestroyCalled;
#endif
};

template<typename... MCs>
class UIControllerWithModuleAccessors : public UIController
{

protected:
    using AccessorList = brigand::list<typename MCs::Accessor...>;
    using AccessorMap = brigand::map<brigand::pair<typename MCs::ModuleType, typename MCs::Accessor>...>;

    template<typename T>
    using GetAccessor = brigand::lookup<AccessorMap, T>;

    using TupleType = brigand::as_tuple<AccessorList>;

public:
    UIControllerWithModuleAccessors()
        : FAccessors(std::move(typename MCs::Accessor(MCs::World))...)
    {
    }

    UIControllerWithModuleAccessors(const UIControllerWithModuleAccessors&) = delete;
    UIControllerWithModuleAccessors(UIControllerWithModuleAccessors&&) = delete;
    UIControllerWithModuleAccessors& operator=(const UIControllerWithModuleAccessors&) = delete;
    UIControllerWithModuleAccessors& operator=(UIControllerWithModuleAccessors&&) = delete;

    virtual ~UIControllerWithModuleAccessors() { }

    void Update() override
    {
        if (!FShow)
            return;

#ifdef PERFORM_SECURITY_CHECKS
        FVirtualUpdateCalled = false;
#endif
        LockDependencies();
        VirtualUpdate();
        UnlockDependencies();
        AlwaysCheckedAssert(FVirtualUpdateCalled);
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
} // namespace UI
} // namespace ECSEngine