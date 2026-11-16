#pragma once
#include "ECSCore/ModuleSystem.h"

#include <vector>

namespace ECSEngine
{
namespace Rendering
{
class DrawCommandBuffer;
class MaterialInstanceHandle;
} // namespace Rendering

class PlanetOrbitalPathFollowingSystem : public ModuleSystem
{
public:
    PlanetOrbitalPathFollowingSystem();

    void DrawDebug(Rendering::DrawCommandBuffer* buffer, const Rendering::MaterialInstanceHandle& material);

protected:
    void VirtualUpdate() override;

private:
    std::vector<std::vector<vec3>> FOrbitRings;
};
} // namespace ECSEngine