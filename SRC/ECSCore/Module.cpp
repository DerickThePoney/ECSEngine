#include "stdafx.h"

#include "Module.h"

namespace ECSEngine
{

Module::Module()
{
}

void Module::Init(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualInitCalled = false;
#endif

    VirtualInit(parUnitId, parParameters);

#ifdef PERFORM_SECURITY_CHECKS
    AlwaysCheckedAssertMsg(FVirtualInitCalled, "You forgot to call the parent's VirtualInit, you naughtyboy !");
#endif
}

void Module::Deinit()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualDeinitCalled = false;
#endif

    VirtualDeinit();

#ifdef PERFORM_SECURITY_CHECKS
    AlwaysCheckedAssertMsg(FVirtualDeinitCalled, "You forgot to call the parent's VirtualInit, you naughtyboy !");
#endif
}

void Module::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    FUnitId = parUnitId;
    AssertRelease(FUnitId.Valid());

#ifdef PERFORM_SECURITY_CHECKS
    FVirtualInitCalled = true;
#endif
}

void Module::VirtualDeinit()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualDeinitCalled = true;
#endif
}

} // namespace ECSEngine