#pragma once
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class ColonyBuildingSystem : public ModuleSystem
{
public:
    ColonyBuildingSystem();
    ~ColonyBuildingSystem();

protected:
    void VirtualUpdate() override;
};

} // namespace ECSEngine