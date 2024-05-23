#pragma once
#include "Common/Serialization.h"

namespace ECSEngine
{
struct PhysicsEngineConfiguration
{
    float FGravityValue = 9.8;

    SERIALIZE() { PROPERTYFIELD(FGravityValue, 9.8); }
};
} // namespace ECSEngine