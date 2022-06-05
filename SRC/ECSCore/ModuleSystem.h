#pragma once
#include "WorldIds_fwd.h"

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
    virtual void VirtualInit();
    virtual void VirtualUpdate();
    virtual void VirtualDestroy();

protected:
    template<class T>
    void RegisterDepency(const EEntityWorlds parEntityWorld);

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
} // namespace ECSEngine
