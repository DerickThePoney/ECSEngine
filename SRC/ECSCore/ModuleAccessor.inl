#pragma once
#include "EntityWorld.h"
#include "ModuleController.h"
#include "WorldManager.h"

namespace ECSEngine
{

template<typename T>
ModuleAccessor<T>::ModuleAccessor(const Worlds::Type parWorld /*= Worlds::STANDARD*/)
{
    AssertRelease(WorldManager::HasInstance());
    EntityWorld* world = WorldManager::Instance().GetWorldIFP(parWorld);
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

template<typename T>
void ECSEngine::ManualLockModuleAccessor<T>::LockIFN()
{
    if (FController->IsLocked())
        return;
    FController->Lock();
}

template<typename T>
void ECSEngine::ManualLockModuleAccessor<T>::UnlockIFN()
{
    if (!FController->IsLocked())
        return;
    FController->Unlock();
}

template<typename T>
ECSEngine::ManualLockModuleAccessor<T>::~ManualLockModuleAccessor()
{
    UnlockIFN();
}

template<typename T>
ECSEngine::ManualLockModuleAccessor<T>::ManualLockModuleAccessor(EntityWorld* world)
{
    FController = dynamic_cast<ModuleController<T>*>(world->GetControllerIFP<T>());
    AssertRelease(FController != nullptr);
}

template<typename T>
ECSEngine::ManualLockModuleAccessor<T>::ManualLockModuleAccessor(const Worlds::Type parWorld /*= Worlds::STANDARD*/)
{
    AssertRelease(WorldManager::HasInstance());
    EntityWorld* world = WorldManager::Instance().GetWorldIFP(parWorld);
    AssertRelease(world != nullptr);
    FController = dynamic_cast<ModuleController<T>*>(world->GetControllerIFP<T>());
    AssertRelease(FController != nullptr);
}

template<typename T>
const T* ECSEngine::ManualLockModuleAccessor<T>::operator[](const EntityId& parId) const
{
    AssertRelease(FController != nullptr);
    AlwaysCheckedAssert(FController->IsLocked());
    return FController->GetModuleForEntity(parId);
}

template<typename T>
T* ECSEngine::ManualLockModuleAccessor<T>::operator[](const EntityId& parId)
{
    AssertRelease(FController != nullptr);
    AlwaysCheckedAssert(FController->IsLocked());
    return FController->GetModuleForEntity(parId);
}

} // namespace ECSEngine
