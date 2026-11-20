#pragma once

#include "Common/Singleton.h"
#include "ECSCore/EntityId.h"

namespace ECSEngine
{
namespace Rendering
{
class DrawCommandBuffer;
class MaterialInstanceHandle;
} // namespace Rendering

struct PlanetNode
{
    EntityId FPlanetId;

    EntityId FParentPlanetId;

    std::vector<EntityId> FPlanetChildren;

    float FSOIRadius = 0.f;
    bool FbSOIInitialised = false;
};

class PlanetarySOIManager : public Singleton<PlanetarySOIManager>
{
public:
    void RegisterPlanet(const EntityId& PlanetId, const EntityId& ParentPlanet);

    bool Update();

    void DrawDebug(Rendering::DrawCommandBuffer* buffer, const Rendering::MaterialInstanceHandle& material);

private:
    void Initialize();

private:
    bool FbInitialized = false;
    std::vector<PlanetNode> FPlanetNodes;
    std::unordered_map<EntityId, u32> FEntityToNodeIdx;
};
} // namespace ECSEngine