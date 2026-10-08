#include "stdafx.h"

#include "PhysicsEngineConfiguration.h"

namespace ECSEngine
{
namespace Physics
{
u32 PhysicsEngineConfiguration::LayerBit(std::string LayerName)
{
    forrange(i, 0, FNumLayers)
    {
        if (FLayers[i] == LayerName)
        {
            return i;
        }
    }

    AssertNotReachedMsg(std::format("PhysicsEngineConfiguration::LayerBit: UNKNOWN layer name {}", LayerName).c_str());
    return u32(-1);
}

const std::string& PhysicsEngineConfiguration::LayerName(u32 LayerBit)
{
    if (LayerBit >= FNumLayers)
    {
        AssertNotReachedMsg(std::format("PhysicsEngineConfiguration::LayerName: UNKNOWN layer bit {}", LayerBit).c_str());
        return "UNKNOWN";
    }
    return FLayers[LayerBit];
}
} // namespace Physics
} // namespace ECSEngine
