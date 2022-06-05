#include "stdafx.h"

#include "ModuleSystem.h"

#include "EntityWorld.h"
#include "ModuleController.h"
#include "ModuleId.h"
#include "WorldManager.h"

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

void ModuleSystem::VirtualInit()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualInitCalled = true;
#endif
}

void ModuleSystem::VirtualUpdate()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualUpdateCalled = true;
#endif
}

void ModuleSystem::VirtualDestroy()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualDestroyCalled = true;
#endif
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

template<class T>
void ModuleSystem::RegisterDepency(const EEntityWorlds parEntityWorld)
{
    AssertRelease(WorldManager::HasInstance());
    EntityWorld* world = WorldManager::Instance().GetWorldIFP(parEntityWorld);
    AssertRelease(world != nullptr);
    IModuleController* controller = world->GetControllerIFP<T>();
    AssertRelease(controller != nullptr);
    FControllers.push_back(controller);
}

#define DECLARE_MODULE_AND_TEMPLATE(NAME, TEMPLATE) template void ModuleSystem::RegisterDepency<NAME>(const EEntityWorlds);
#include "ModuleList.inl"
#undef DECLARE_MODULE_AND_TEMPLATE

} // namespace ECSEngine
