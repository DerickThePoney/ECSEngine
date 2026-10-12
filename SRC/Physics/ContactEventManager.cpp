#include "stdafx.h"

#include "ContactEventManager.h"

#include "Common/Logger.h"
#include "ContactEvent.h"

namespace ECSEngine
{
namespace Physics
{
void ContactEventEventManager::RegisterEntityHandleCorrespondance(const EntityId& unitID, PhysicsBodyHandle& bodyHandle)
{
    FPhysicsBodyHandleToEntityId.insert_or_assign(bodyHandle, unitID);
    FEntityIdToPhysicsBodyHandle.insert_or_assign(unitID, bodyHandle);
}

void ContactEventEventManager::UnegisterEntityHandleCorrespondance(const EntityId& unitID)
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

void ContactEventEventManager::AddContactEventListener(const EntityId& unitID, BeginContactDelegate* Begin, EndContactDelegate* End)
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

void ContactEventEventManager::RemoveContactEventListener(const EntityId& unitID, BeginContactDelegate* Begin, EndContactDelegate* End)
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

void ContactEventEventManager::DispatchContactEvents(const std::vector<ContactEvent>& contactEvents)
{
    // Might handle to unitID translation here to actually be usefull...
    foreachitemconst(evt, contactEvents)
    {
        switch (evt.Type)
        {
        case EContactEventType::Begin:
        {
            auto itFind = FBeginContactListeners.find(evt.BodyA);
            if (itFind != FBeginContactListeners.end())
                itFind->second(evt);

            itFind = FBeginContactListeners.find(evt.BodyB);
            if (itFind != FBeginContactListeners.end())
                itFind->second(evt);
            break;
        }
        case EContactEventType::End:
        {
            auto itFind = FEndContactListeners.find(evt.BodyA);
            if (itFind != FEndContactListeners.end())
                itFind->second(evt);

            itFind = FEndContactListeners.find(evt.BodyB);
            if (itFind != FEndContactListeners.end())
                itFind->second(evt);
            break;
        }
        }
    }
}

} // namespace Physics
} // namespace ECSEngine