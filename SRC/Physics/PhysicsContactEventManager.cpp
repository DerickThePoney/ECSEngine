#include "stdafx.h"

#include "PhysicsContactEventManager.h"

#include "Common/Logger.h"
#include "ContactEvent.h"

namespace ECSEngine
{
namespace Physics
{
void PhysicsContactEventManager::RegisterEntityHandleCorrespondance(const EntityId& unitID, PhysicsBodyHandle& bodyHandle)
{
    FPhysicsBodyHandleToEntityId.insert_or_assign(bodyHandle, unitID);
    FEntityIdToPhysicsBodyHandle.insert_or_assign(unitID, bodyHandle);
}

void PhysicsContactEventManager::UnegisterEntityHandleCorrespondance(const EntityId& unitID)
{
    auto itFind = FEntityIdToPhysicsBodyHandle.find(unitID);
    if (itFind == FEntityIdToPhysicsBodyHandle.end())
        return;
    const PhysicsBodyHandle& handle = itFind->second;

    FPhysicsBodyHandleToEntityId.erase(handle);
    FEntityIdToPhysicsBodyHandle.erase(unitID);
    FBeginContactListeners.erase(handle);
    FEndContactListeners.erase(handle);
}

void PhysicsContactEventManager::AddContactEventListener(const EntityId& unitID, BeginContactDelegate* Begin, EndContactDelegate* End)
{
    auto itFind = FEntityIdToPhysicsBodyHandle.find(unitID);
    if (itFind == FEntityIdToPhysicsBodyHandle.end())
    {
#ifdef PERFORM_SECURITY_CHECKS
        LOG_PHYSICS(std::format("Trying to register listeners for unregestered unitID {}", unitID));
#endif
        return;
    }
    const PhysicsBodyHandle& handle = itFind->second;

    if (Begin != nullptr)
        FBeginContactListeners.insert_or_assign(handle, *Begin);

    if (End != nullptr)
        FEndContactListeners.insert_or_assign(handle, *End);
}

void PhysicsContactEventManager::RemoveContactEventListener(const EntityId& unitID, BeginContactDelegate* Begin, EndContactDelegate* End)
{
    auto itFind = FEntityIdToPhysicsBodyHandle.find(unitID);
    if (itFind == FEntityIdToPhysicsBodyHandle.end())
    {
#ifdef PERFORM_SECURITY_CHECKS
        LOG_PHYSICS(std::format("Trying to register listeners for unregestered unitID {}", unitID));
#endif
        return;
    }
    const PhysicsBodyHandle& handle = itFind->second;
    FBeginContactListeners.erase(handle);
    FEndContactListeners.erase(handle);
}

void PhysicsContactEventManager::DispatchContactEvents(const ContactEventArray& contactEvents, u32 ContactEventCount)
{
    forrange(i, 0, ContactEventCount)
    {
        const ContactEvent& evt = contactEvents[i];

        // Convert body handles to entityID
        auto itUnit = FPhysicsBodyHandleToEntityId.find(evt.BodyA);
        AlwaysCheckedAssert(itUnit != FPhysicsBodyHandleToEntityId.end());
        if (itUnit == FPhysicsBodyHandleToEntityId.end())
        {
            continue;
        }
        const EntityId& unitA = itUnit->second;

        itUnit = FPhysicsBodyHandleToEntityId.find(evt.BodyB);
        AlwaysCheckedAssert(itUnit != FPhysicsBodyHandleToEntityId.end());
        if (itUnit == FPhysicsBodyHandleToEntityId.end())
        {
            continue;
        }
        const EntityId& unitB = itUnit->second;

        // Dispatch events
        switch (evt.Type)
        {
        case EContactEventType::Begin:
        {
            auto itFind = FBeginContactListeners.find(evt.BodyA);
            if (itFind != FBeginContactListeners.end())
                itFind->second(unitA, unitB, evt);

            itFind = FBeginContactListeners.find(evt.BodyB);
            if (itFind != FBeginContactListeners.end())
                itFind->second(unitA, unitB, evt);
            break;
        }
        case EContactEventType::End:
        {
            auto itFind = FEndContactListeners.find(evt.BodyA);
            if (itFind != FEndContactListeners.end())
                itFind->second(unitA, unitB, evt);

            itFind = FEndContactListeners.find(evt.BodyB);
            if (itFind != FEndContactListeners.end())
                itFind->second(unitA, unitB, evt);
            break;
        }
        }
    }
}

} // namespace Physics
} // namespace ECSEngine