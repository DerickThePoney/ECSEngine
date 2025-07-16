#pragma once
#include "Common/BitSet.h"
#include "Common/PoolAllocator.h"
#include "PhysicsBodyHandle.h"

namespace ECSEngine
{
namespace Physics
{
struct Contact;
struct ContactEdge
{
    PhysicsBodyHandle FOtherBody;

    Contact* FContact = nullptr;

    ContactEdge* FPrev = nullptr;
    ContactEdge* FNext = nullptr;
};

enum EContactFlag
{
    TOUCHING = 0x01,
    ISLAND = 0x02,
    CONTACT_INFO = 0x04, // temp value
    COUNT
};

struct Contact
{
    DECLARE_POOL_ALLOCATED(Contact);

public:
    void Evaluate();

public:
    PhysicsBodyHandle FFirstBody;
    PhysicsBodyHandle FSecondBody;

    // Contact data
    ContactEdge FFirstBodyEdge;
    ContactEdge FSecondBodyEdge;

    // FreeList implementation
    Contact* FNext = nullptr;
    Contact* FPrev = nullptr;

    // NEEDS LOTS OF OTHER STUFFS
    vec3 FContactNormal;
    vec3 FContactPoint;
    float FPenetration = 0.f;

    BitSet<EContactFlag::COUNT> FFlags;
};
} // namespace Physics
} // namespace ECSEngine