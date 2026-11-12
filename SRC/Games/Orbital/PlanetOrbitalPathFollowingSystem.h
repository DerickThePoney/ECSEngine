#pragma once
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class PlanetOrbitalPathFollowingSystem : public ModuleSystem
{
public:
    PlanetOrbitalPathFollowingSystem();

protected:
    void VirtualUpdate() override;
};
} // namespace ECSEngine