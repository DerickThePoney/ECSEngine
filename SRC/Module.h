#pragma once
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
    Module() {}
    virtual ~Module() {}

public:
    void Init(const ModuleParameters::ParameterContainer& parParameters);
    void Deinit();

protected:
    virtual void VirtualInit(const ModuleParameters::ParameterContainer& parParameters);
    virtual void VirtualDeinit();

private:
#ifdef PERFORM_SECURITY_CHECKS
    bool FVirtualInitCalled = false;
    bool FVirtualDeinitCalled = false;
#endif
};

#define DECLARE_MODULE(TYPE)                                                                                                                                                       \
public:                                                                                                                                                                            \
    constexpr u32 GetModuleId() const { return ModuleTraits<TYPE>::GetModuleId(); }                                                                                                \
    using parent_type = Module;
} // namespace ECSEngine