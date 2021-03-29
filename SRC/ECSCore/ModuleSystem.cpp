#include "stdafx.h"

#include "ModuleSystem.h"

#include "ModuleController.h"

namespace ECSEngine
{
ModuleSystem::ModuleSystem()
#ifdef PERFORM_SECURITY_CHECKS
    : FVirtualUpdateCalled(false)
    , FVirtualInitCalled(false)
    , FVirtualDestroyCalled(false)
#endif // PERFORM_SECURITY_CHECKS

{
}

ModuleSystem::~ModuleSystem()
{
    FControllers.clear();
}

void ModuleSystem::Init()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualInitCalled = false;
#endif

    VirtualInit();

    AlwaysCheckedAssert(FVirtualInitCalled == true);
}

void ModuleSystem::Update()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualUpdateCalled = false;
#endif
    LockControllers();

    VirtualUpdate();

    AlwaysCheckedAssert(FVirtualUpdateCalled == true);
    UnlockControllers();
}

void ModuleSystem::Destroy()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualDestroyCalled = false;
#endif

    VirtualDestroy();

    AlwaysCheckedAssert(FVirtualDestroyCalled == true);
}

void ModuleSystem::LockControllers()
{
    for (u32 i = 0; i < FControllers.size(); ++i)
    {
        IModuleController*& controller = FControllers[i];
        AlwaysCheckedAssert(!controller->IsLocked());
        controller->Lock();
        AlwaysCheckedAssert(controller->IsLocked());
    }
}

void ModuleSystem::UnlockControllers()
{
    for (u32 i = 0; i < FControllers.size(); ++i)
    {
        IModuleController*& controller = FControllers[i];
        AlwaysCheckedAssert(controller->IsLocked());
        controller->Unlock();
        AlwaysCheckedAssert(!controller->IsLocked());
    }
}

} // namespace ECSEngine
