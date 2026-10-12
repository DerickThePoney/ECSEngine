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
public:
    PhysicsBodyHandle BodyA;
    PhysicsBodyHandle BodyB;

    std::vector<ContactPoint> ContactPoints;
    vec3 Normal;

    EContactEventType Type;
};

constexpr u32 MaxContactEvents = 1024;
using ContactEventArray = std::array<ContactEvent, MaxContactEvents>;
} // namespace Physics
} // namespace ECSEngine