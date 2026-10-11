#pragma once
#include "ECSCore/EntityId.h"
#include "PhysicsBodyHandle.h"

namespace ECSEngine
{

namespace Physics
{
struct ContactEvent;
using BeginContactDelegate = Delegate<void(const ContactEvent& evt)>;
using EndContactDelegate = Delegate<void(const ContactEvent& evt)>;

class ContactEventEventManager
{
public:
    void RegisterEntityHandleCorrespondance(const EntityId& unitID, PhysicsBodyHandle& bodyHandle);
    void UnegisterEntityHandleCorrespondance(const EntityId& unitID);
    void AddContactEventListener(const EntityId& unitID, BeginContactDelegate* Begin = nullptr, EndContactDelegate* End = nullptr);
    void RemoveContactEventListener(const EntityId& unitID, BeginContactDelegate* Begin = nullptr, EndContactDelegate* End = nullptr);

    void DispatchContactEvents(const std::vector<ContactEvent>& contactEvents);

private:
    std::unordered_map<PhysicsBodyHandle, EntityId> FPhysicsBodyHandleToEntityId;
    std::unordered_map<EntityId, PhysicsBodyHandle> FEntityIdToPhysicsBodyHandle;
    std::unordered_map<PhysicsBodyHandle, BeginContactDelegate> FBeginContactListeners;
    std::unordered_map<PhysicsBodyHandle, EndContactDelegate> FEndContactListeners;
};
} // namespace Physics
} // namespace ECSEngine