#pragma once
#include "Constants.h"
#include "EntityId.h"
#include "ModulePoolAllocator.h"

namespace ECSEngine
{
class IModuleController
{
public:
    virtual void AllocateForEntity(const EntityId& parEntity) = 0;
    virtual void DeallocateForEntity(const EntityId& parEntity) = 0;
};

template<class Mod>
class ModuleController final : public IModuleController
{
public:
    ModuleController()
        : FLock(false)
    {
    }

    void AllocateForEntity(const EntityId& parEntity)
    {
        Lock();
        const u32 entitySequentialId = parEntity.GetSequentialId();
        AssertRelease(parEntity.Valid() && parEntity.GetSequentialId() < ModulePoolSize);
        FAllocator.AllocateAtIndex(entitySequentialId);
        FAllocatedModules.insert(parEntity);
        Unlock();
    }

    void DeallocateForEntity(const EntityId& parEntity)
    {
        Lock();
        const u32 entitySequentialId = parEntity.GetSequentialId();
        AssertRelease(parEntity.Valid() && parEntity.GetSequentialId() < ModulePoolSize);
        auto itFind = FAllocatedModules.find(parEntity);
        AlwaysCheckedAssert(itFind != FAllocatedModules.end());
        FAllocator.DeallocateAtIndex(entitySequentialId);
        FAllocatedModules.erase(itFind);
        Unlock();
    }

    Mod* GetModuleForEntity(const EntityId& parEntity)
    {
        AlwaysCheckedAssert(parEntity.Valid());
        if (FAllocatedModules.find(parEntity) != FAllocatedModules.end())
        {
            return FAllocator.GetAtIndex(parEntity.GetSequentialId());
        }
        return nullptr;
    }

    const Mod* GetModuleForEntity(const EntityId& parEntity) const
    {
        AlwaysCheckedAssert(parEntity.Valid());
        if (FAllocatedModules.find(parEntity) != FAllocatedModules.end())
        {
            return FAllocator.GetAtIndex(parEntity.GetSequentialId());
        }
        return nullptr;
    }

    void Lock()
    {
        AlwaysCheckedAssert(FLock == false);
        FLock = true;
    }

    void Unlock()
    {
        AlwaysCheckedAssert(FLock == true);
        FLock = false;
    }

    bool IsLocked() const { return FLock; }

    std::set<EntityId>::iterator begin()
    {
        AlwaysCheckedAssert(IsLocked());
        return FAllocatedModules.begin();
    }
    std::set<EntityId>::const_iterator begin() const
    {
        AlwaysCheckedAssert(IsLocked());
        return FAllocatedModules.begin();
    }
    std::set<EntityId>::reverse_iterator rbegin()
    {
        AlwaysCheckedAssert(IsLocked());
        return FAllocatedModules.rbegin();
    }
    std::set<EntityId>::const_reverse_iterator rbegin() const
    {
        AlwaysCheckedAssert(IsLocked());
        return FAllocatedModules.rbegin();
    }

    std::set<EntityId>::iterator end()
    {
        AlwaysCheckedAssert(IsLocked());
        return FAllocatedModules.end();
    }
    std::set<EntityId>::const_iterator end() const
    {
        AlwaysCheckedAssert(IsLocked());
        return FAllocatedModules.end();
    }
    std::set<EntityId>::reverse_iterator rend()
    {
        AlwaysCheckedAssert(IsLocked());
        return FAllocatedModules.rend();
    }
    std::set<EntityId>::const_reverse_iterator rend() const
    {
        AlwaysCheckedAssert(IsLocked());
        return FAllocatedModules.rend();
    }

private:
    ModulePoolAllocator<Mod, ModulePoolSize, true> FAllocator;
    std::set<EntityId> FAllocatedModules;
    std::atomic_bool FLock;
};
} // namespace ECSEngine
