#pragma once
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class PhysicsUpdateSystem final : public ModuleSystem
{
    using parent_type = ModuleSystem;

public:
    PhysicsUpdateSystem();
    ~PhysicsUpdateSystem();

    void ExternalSyncTransformsFromPhysics();

protected:
    void VirtualUpdate() override;

    void SyncTransformsFromPhysics();
};
} // namespace ECSEngine
