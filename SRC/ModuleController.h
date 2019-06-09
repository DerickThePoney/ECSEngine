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

    template<class Iterator>
    class ModuleControllerIterator
    {
    public:
        ModuleControllerIterator(ModuleController<Mod>* parController, Iterator parIt)
            : FController(parController)
            , FIt(parIt)
        {
            AssertRelease(FController != nullptr);
        }

        bool operator==(const ModuleControllerIterator<Iterator>& other) { return FIt == other.FIt; }
        bool operator!=(const ModuleControllerIterator<Iterator>& other) { return FIt != other.FIt; }

        void operator++(int) { FIt++; }
        void operator++() { ++FIt; }

        Mod* operator*()
        {
            AssertRelease(FController != nullptr);
            return FController->GetModuleForEntity(*FIt);
        }

    private:
        Iterator FIt;
        ModuleController<Mod>* FController;
    };

    using iterator = typename ModuleControllerIterator<std::set<EntityId>::iterator>;
    // using const_iterator = typename ModuleControllerIterator<std::set<EntityId>::const_iterator>;
    using reverse_iterator = typename ModuleControllerIterator<std::set<EntityId>::reverse_iterator>;
    // using const_reverse_iterator = typename ModuleControllerIterator<std::set<EntityId>::const_reverse_iterator>;

    iterator begin()
    {
        AlwaysCheckedAssert(IsLocked());
        return iterator(this, FAllocatedModules.begin());
    }
    /*const_iterator begin() const
    {
        AlwaysCheckedAssert(IsLocked());
        return const_iterator(this, FAllocatedModules.begin());
    }*/
    reverse_iterator rbegin()
    {
        AlwaysCheckedAssert(IsLocked());
        return reverse_iterator(this, FAllocatedModules.rbegin());
    }
    /*const_reverse_iterator rbegin() const
    {
        AlwaysCheckedAssert(IsLocked());
        return const_reverse_iterator(this, FAllocatedModules.rbegin());
    }*/

    iterator end()
    {
        AlwaysCheckedAssert(IsLocked());
        return iterator(this, FAllocatedModules.end());
    }
    /*const_iterator end() const
    {
        AlwaysCheckedAssert(IsLocked());
        return iterator(this, FAllocatedModules.end());
    }*/
    reverse_iterator rend()
    {
        AlwaysCheckedAssert(IsLocked());
        return reverse_iterator(this, FAllocatedModules.rend());
    }
    /*const_reverse_iterator rend() const
    {
        AlwaysCheckedAssert(IsLocked());
        return const_reverse_iterator(this, FAllocatedModules.rend());
    }*/

private:
    ModulePoolAllocator<Mod, ModulePoolSize, true> FAllocator;
    std::set<EntityId> FAllocatedModules;
    std::atomic_bool FLock;
};
} // namespace ECSEngine
