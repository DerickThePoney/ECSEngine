#pragma once
#include "EntityWorld.h"
#include "ModuleController.h"
#include "WorldManager.h"

namespace ECSEngine
{

template<typename T>
ModuleAccessor<T>::ModuleAccessor()
{
    AssertRelease(WorldManager::HasInstance());
    EntityWorld* world = WorldManager::Instance().GetWorldIFP(Worlds::STANDARD);
    AssertRelease(world != nullptr);
    FController = dynamic_cast<ModuleController<T>*>(world->GetControllerIFP<T>());
    AssertRelease(FController != nullptr);
    AlwaysCheckedAssert(FController->IsLocked());
}

template<typename T>
ModuleAccessor<T>::ModuleAccessor(EntityWorld* world)
{
    FController = dynamic_cast<ModuleController<T>*>(world->GetControllerIFP<T>());
    AssertRelease(FController != nullptr);
    AlwaysCheckedAssert(FController->IsLocked());
}

template<typename T>
const T* ECSEngine::ModuleAccessor<T>::operator[](const EntityId& parId) const
{
    AssertRelease(FController != nullptr);
    AlwaysCheckedAssert(FController->IsLocked());
    return FController->GetModuleForEntity(parId);
}

template<typename T>
T* ECSEngine::ModuleAccessor<T>::operator[](const EntityId& parId)
{
    AssertRelease(FController != nullptr);
    AlwaysCheckedAssert(FController->IsLocked());
    return FController->GetModuleForEntity(parId);
}

} // namespace ECSEngine
