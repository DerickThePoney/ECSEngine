#pragma once
#include "Common/Serialization.h"

namespace ECSEngine
{
namespace Physics
{
struct PhysicsEngineConfiguration
{
    float FGravityValue = 9.8;

    SERIALIZE() { PROPERTYFIELD(GravityValue, 9.8); }
};
} // namespace Physics
} // namespace ECSEngine