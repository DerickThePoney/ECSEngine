#include "stdafx.h"

#include "PhysicsMoveabilityEnum.h"

namespace ECSEngine
{
namespace Physics
{
namespace EPhysicsMoveability
{
std::string AsString(Type value)
{
    switch (value)
    {
    case ECSEngine::Physics::EPhysicsMoveability::STATIC:
        return "STATIC";
        break;
    case ECSEngine::Physics::EPhysicsMoveability::KINEMATIC:
        return "KINEMATIC";
        break;
    case ECSEngine::Physics::EPhysicsMoveability::PHYICS_ENABLED:
        return "PHYSICS_ENABLED";
        break;
    case ECSEngine::Physics::EPhysicsMoveability::LENGTH:
    default:
        AssertNotReached();
        return "UNKNOWN";
        break;
    }
}
} // namespace EPhysicsMoveability
} // namespace Physics
} // namespace ECSEngine
