#pragma once
#include "Common/PoolAllocator.h"
#include "Contact.h"
#include "PhysicsBodyHandle.h"

namespace ECSEngine
{
namespace Physics
{

enum class EContactEventType : u8
{
    Begin,
    End
};

struct ContactEvent
{
    DECLARE_POOL_ALLOCATED(ContactEvent);

public:
    PhysicsBodyHandle BodyA;
    PhysicsBodyHandle BodyB;

    std::vector<ContactPoint> ContactPoints;
    vec3 Normal;

    EContactEventType Type;
};
} // namespace Physics
} // namespace ECSEngine