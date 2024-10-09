#pragma once
#include "Common/PoolAllocator.h"
#include "PhysicsBodyHandle.h"

namespace ECSEngine
{
namespace Physics
{
class BroadPhase;
struct Contact
{
    DECLARE_POOL_ALLOCATED(Contact);

    // Contact data
    PhysicsBodyHandle BodyA;
    PhysicsBodyHandle BodyB;

    // NEEDS LOTS OF OTHER STUFFS

    // FreeList implementation
    Contact* FNext = nullptr;
    Contact* FPrev = nullptr;
};

class ContactManager
{
public:
    void FindNewContacts(BroadPhase& parBroadPhase);
};
} // namespace Physics
} // namespace ECSEngine