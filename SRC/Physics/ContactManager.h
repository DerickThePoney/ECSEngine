#pragma once
#include "Common/BitSet.h"
#include "Common/PoolAllocator.h"
#include "PhysicsBodyHandle.h"

namespace ECSEngine
{
namespace Physics
{
class BroadPhase;

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

class ContactManager
{
public:
    ~ContactManager();
    void FindNewContacts(BroadPhase& parBroadPhase);

    void CollideContacts(BroadPhase& parBroadPhase);

#ifdef PERFORM_SECURITY_CHECKS
    void DebugDrawContacts(BroadPhase& parBroadPhase);
#endif

private:
    void AddPotentialContactPair(const PhysicsBodyHandle& first, const PhysicsBodyHandle& second);

    void DestroyContact(Contact* c);

private:
    Contact* FContactList = nullptr;
    i32 FContactCount;
};
} // namespace Physics
} // namespace ECSEngine