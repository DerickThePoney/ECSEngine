#pragma once
#include "PhysicsBodyHandle.h"

namespace ECSEngine
{
namespace Physics
{
class BroadPhase;
class PhysicsEngine;
struct Contact;

class ContactManager
{
public:
    ~ContactManager();

    void RemoveBody(const PhysicsBodyHandle& Handle);
    void Shutdown();

    void FindNewContacts(BroadPhase& parBroadPhase);

    void CollideContacts(PhysicsEngine* Engine, BroadPhase& parBroadPhase);

#ifdef PERFORM_SECURITY_CHECKS
    void DebugDrawContacts(BroadPhase& parBroadPhase);
#endif

    i32 ContactCount() const { return FContactCount; }

private:
    void AddPotentialContactPair(const PhysicsBodyHandle& first, const PhysicsBodyHandle& second);

    void DestroyContact(Contact* c);

private:
    Contact* FContactList = nullptr;
    i32 FContactCount;
};
} // namespace Physics
} // namespace ECSEngine