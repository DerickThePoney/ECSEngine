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

    void SyncTransformsFromPhysics();

protected:
    void VirtualUpdate() override;
};
} // namespace ECSEngine
