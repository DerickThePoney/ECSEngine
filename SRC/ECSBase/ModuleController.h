#pragma once
#include "Common/Constants.h"
#include "Common/ObjectPoolAllocator.h"
#include "EntityId.h"

namespace ECSEngine
{
class Module;

class IModuleController
{
public:
    virtual void Lock() = 0;
    virtual void Unlock() = 0;
    virtual bool IsLocked() const = 0;
    virtual void AllocateForEntity(const EntityId& parEntity) = 0;
    virtual void DeallocateForEntity(const EntityId& parEntity) = 0;
    virtual Module* GetModulePtrForEntity(const EntityId& parEntity) = 0;
};

template<class Mod>
class ModuleController final : public IModuleController
{
public:
    ModuleController()
        : FLock(false)
    {
    }

    void AllocateForEntity(const EntityId& parEntity) override
    {
        Lock();
        const u32 entitySequentialId = parEntity.GetSequentialId();
        AssertRelease(parEntity.Valid() && parEntity.GetSequentialId() < ModulePoolSize);
        FAllocator.AllocateAtIndex(entitySequentialId);
        FAllocatedModules.insert(parEntity);
        Unlock();
    }

    void DeallocateForEntity(const EntityId& parEntity) override
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

    Module* GetModulePtrForEntity(const EntityId& parEntity) override
    {
        AlwaysCheckedAssert(parEntity.Valid());
        if (FAllocatedModules.find(parEntity) != FAllocatedModules.end())
        {
            return FAllocator.GetAtIndex(parEntity.GetSequentialId());
        }
        return nullptr;
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

    void Lock() override
    {
        AlwaysCheckedAssert(FLock == false);
        FLock = true;
    }

    void Unlock() override
    {
        AlwaysCheckedAssert(FLock == true);
        FLock = false;
    }

    bool IsLocked() const override { return FLock; }

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

        Mod& operator*() { return GetRef(); }
        Mod& operator->() = delete;
        Mod& GetRef()
        {
            AssertRelease(FController != nullptr);
            return *FController->GetModuleForEntity(*FIt);
        }

    private:
        Iterator FIt;
        ModuleController<Mod>* FController;
    };

    template<class Iterator>
    class ModuleControllerConstIterator
    {
    public:
        ModuleControllerConstIterator(ModuleController<Mod>* parController, Iterator parIt)
            : FController(parController)
            , FIt(parIt)
        {
            AssertRelease(FController != nullptr);
        }

        bool operator==(const ModuleControllerConstIterator<Iterator>& other) { return FIt == other.FIt; }
        bool operator!=(const ModuleControllerConstIterator<Iterator>& other) { return FIt != other.FIt; }

        void operator++(int) { FIt++; }
        void operator++() { ++FIt; }

        const Mod& operator*() { return GetRef(); }
        const Mod& operator->() = delete;

        const Mod& GetRef()
        {
            AssertRelease(FController != nullptr);
            return *FController->GetModuleForEntity(*FIt);
        }

    private:
        Iterator FIt;
        ModuleController<Mod>* FController;
    };

    using iterator = typename ModuleControllerIterator<std::set<EntityId>::iterator>;
    using const_iterator = typename ModuleControllerConstIterator<std::set<EntityId>::const_iterator>;
    using reverse_iterator = typename ModuleControllerIterator<std::set<EntityId>::reverse_iterator>;
    using const_reverse_iterator = typename ModuleControllerConstIterator<std::set<EntityId>::const_reverse_iterator>;

    iterator begin()
    {
        AlwaysCheckedAssert(IsLocked());
        return iterator(this, FAllocatedModules.begin());
    }
    const_iterator cbegin()
    {
        AlwaysCheckedAssert(IsLocked());
        return const_iterator(this, FAllocatedModules.cbegin());
    }
    reverse_iterator rbegin()
    {
        AlwaysCheckedAssert(IsLocked());
        return reverse_iterator(this, FAllocatedModules.rbegin());
    }
    const_reverse_iterator crbegin()
    {
        AlwaysCheckedAssert(IsLocked());
        return const_reverse_iterator(this, FAllocatedModules.crbegin());
    }

    iterator end()
    {
        AlwaysCheckedAssert(IsLocked());
        return iterator(this, FAllocatedModules.end());
    }
    const_iterator cend()
    {
        AlwaysCheckedAssert(IsLocked());
        return const_iterator(this, FAllocatedModules.cend());
    }
    reverse_iterator rend()
    {
        AlwaysCheckedAssert(IsLocked());
        return reverse_iterator(this, FAllocatedModules.rend());
    }
    const_reverse_iterator crend()
    {
        AlwaysCheckedAssert(IsLocked());
        return const_reverse_iterator(this, FAllocatedModules.crend());
    }

private:
    ObjectPoolAllocator<Mod, ModulePoolSize, true> FAllocator;
    std::set<EntityId> FAllocatedModules;
    std::atomic_bool FLock;
};
} // namespace ECSEngine
