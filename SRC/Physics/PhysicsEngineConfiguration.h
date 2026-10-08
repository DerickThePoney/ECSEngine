#pragma once
#include "Common/Serialization.h"

namespace ECSEngine
{
namespace Physics
{
struct PhysicsEngineConfiguration
{
    using LayersArray = std::array<std::string, 32>;

    float FGravityValue = 9.8f;
    float FAABBFattenValue = 1.f;
    float FAABBDisplacementMultiplier = 4.f;

    LayersArray FLayers;
    u32 FNumLayers = 0;

    SERIALIZE()
    {
        PROPERTYFIELD(GravityValue, 9.8f);
        PROPERTYFIELD(AABBFattenValue, 1.f);
        PROPERTYFIELD(AABBDisplacementMultiplier, 1.f);
        PROPERTYFIELD(Layers, LayersArray());
        PROPERTYFIELD(NumLayers, 0);
    }
};
} // namespace Physics
} // namespace ECSEngine