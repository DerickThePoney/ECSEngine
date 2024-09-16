#pragma once
#include "Common/Serialization.h"

namespace ECSEngine
{
namespace Physics
{
struct PhysicsEngineConfiguration
{
    float FGravityValue = 9.8f;
    float FAABBFattenValue = 1.f;
    float FAABBDisplacementMultiplier = 4.f;

    SERIALIZE()
    {
        PROPERTYFIELD(GravityValue, 9.8f);
        PROPERTYFIELD(AABBFattenValue, 1.f);
        PROPERTYFIELD(AABBDisplacementMultiplier, 1.f);
    }
};
} // namespace Physics
} // namespace ECSEngine