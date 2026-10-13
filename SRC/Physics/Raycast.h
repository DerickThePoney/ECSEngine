#pragma once

#include "Common/Ray.h"
#include "ECSCore/EntityId.h"
#include "Math/VectorTypes.h"
#include "PhysicsBodyHandle.h"

namespace ECSEngine
{
namespace Physics
{
struct RaycastHit
{
    PhysicsBodyHandle BodyHandle;
    EntityId UnitId = EntityId(0xFF, 0xFFFFFF);

    float Distance = std::numeric_limits<float>::max();
    vec3 Point;
    vec3 Normal;
};

struct RaycastQuery
{
    Ray3D Ray;

    // Max distance, -1 means no clipping.
    float MaxDistance = -1.f;

    std::string Preset;
};

bool RaycastClosest(const RaycastQuery& Query, RaycastHit& outHit);
} // namespace Physics
} // namespace ECSEngine