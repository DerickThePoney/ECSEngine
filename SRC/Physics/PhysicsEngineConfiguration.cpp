#include "stdafx.h"

#include "PhysicsEngineConfiguration.h"

namespace ECSEngine
{
namespace Physics
{
u32 PhysicsEngineConfiguration::LayerBit(std::string LayerName) const
{
    forrange(i, 0, FNumLayers)
    {
        if (FLayers[i] == LayerName)
        {
            return i;
        }
    }

    AssertNotReachedMsg(std::format("PhysicsEngineConfiguration::LayerBit: UNKNOWN layer name {} - reverting to default", LayerName).c_str());
    return u32(1);
}

static std::string UNKOWNLAYER("UNKNOWN");
const std::string& PhysicsEngineConfiguration::LayerName(u32 LayerBit) const
{
    if (LayerBit >= FNumLayers)
    {
        AssertNotReachedMsg(std::format("PhysicsEngineConfiguration::LayerName: UNKNOWN layer bit {}", LayerBit).c_str());
        return UNKOWNLAYER;
    }
    return FLayers[LayerBit];
}
} // namespace Physics
} // namespace ECSEngine
