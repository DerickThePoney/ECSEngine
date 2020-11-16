#pragma once
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class ResourceProductionSystem final : public ModuleSystem
{
public:
    ResourceProductionSystem();
    ~ResourceProductionSystem();

protected:
    void VirtualUpdate() override;
};
} // namespace ECSEngine
