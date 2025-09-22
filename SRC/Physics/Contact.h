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

enum EContactFlag : u32
{
    CT_TOUCHING,
    CT_WAS_TOUCHING,
    CT_ISLAND,
    CT_CONTACT_INFO, // temp value
    CT_COUNT
};

struct ContactManifold
{
    std::vector<vec3> FPositions;
    std::vector<float> FPenetration;

    void clear()
    {
        FPositions.clear();
        FPenetration.clear();
    }
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

    //
    ContactManifold FManifold;
    vec3 FContactNormal;

    BitSet<EContactFlag::CT_COUNT> FFlags;

#ifdef PERFORM_SECURITY_CHECKS
    vec3 SeparatingAxis;
    vec3 SeparationVector;
#endif
};
} // namespace Physics
} // namespace ECSEngine