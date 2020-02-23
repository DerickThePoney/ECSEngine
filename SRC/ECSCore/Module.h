#pragma once
#include "EntityId.h"
#include "ModuleId.h"

namespace ECSEngine
{
namespace ModuleParameters
{
class ParameterContainer;
}
} // namespace ECSEngine

namespace ECSEngine
{
class Module
{
public:
protected:
    Module();
    virtual ~Module() {}

public:
    void Init(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters);
    void Deinit();

    virtual u32 GetModuleId() const
    {
        AssertNotReached();
        return -1;
    }

    const EntityId& UnitId() const { return FUnitId; }

    template<typename T>
    const T* Template();

protected:
    virtual void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters);
    virtual void VirtualDeinit();

private:
    EntityId FUnitId;
#ifdef PERFORM_SECURITY_CHECKS
    bool FVirtualInitCalled = false;
    bool FVirtualDeinitCalled = false;
#endif
};

#define DECLARE_MODULE(TYPE)                                                                                                                                                       \
public:                                                                                                                                                                            \
    u32 GetModuleId() const override { return ModuleTraits<TYPE>::GetModuleId(); }                                                                                                 \
    using parent_type = Module;
} // namespace ECSEngine

#include "Module.inl"