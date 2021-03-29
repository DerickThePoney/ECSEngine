#pragma once
#include "EntityWorld.h"
#include "WorldManager.h"

namespace ECSEngine
{
class IModuleController;

class ModuleSystem
{
public:
    ModuleSystem();
    virtual ~ModuleSystem();

    void Init();
    void Update();
    void Destroy();

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
    template<class T>
    void RegisterDepency(const Worlds::Type parEntityWorld);

protected:
    void LockControllers();
    void UnlockControllers();

protected:
    std::vector<IModuleController*> FControllers;

#ifdef PERFORM_SECURITY_CHECKS
    bool FVirtualInitCalled;
    bool FVirtualUpdateCalled;
    bool FVirtualDestroyCalled;
#endif
};

template<class T>
void ModuleSystem::RegisterDepency(const Worlds::Type parEntityWorld)
{
    AssertRelease(WorldManager::HasInstance());
    EntityWorld* world = WorldManager::Instance().GetWorldIFP(parEntityWorld);
    AssertRelease(world != nullptr);
    IModuleController* controller = world->GetControllerIFP<T>();
    AssertRelease(controller != nullptr);
    FControllers.push_back(controller);
}

} // namespace ECSEngine
