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
    BitSet<EContactFlag::COUNT> FFlags;
};
} // namespace Physics
} // namespace ECSEngine