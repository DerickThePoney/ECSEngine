#pragma once
#include "EntityId.h"

namespace ECSEngine
{
class EntityWorld;
template<typename T>
class ModuleController;

template<typename T>
class ModuleAccessor
{
public:
    ModuleAccessor(EntityWorld* world);

    T* operator[](const EntityId& parId);
    const T* operator[](const EntityId& parId) const;

    std::set<EntityId>::iterator begin()
    {
        AssertRelease(FController != nullptr);
        return FController->begin();
    }
    std::set<EntityId>::const_iterator begin() const
    {
        AssertRelease(FController != nullptr);
        return FController->begin();
    }
    std::set<EntityId>::reverse_iterator rbegin()
    {
        AssertRelease(FController != nullptr);
        return FController->rbegin();
    }
    std::set<EntityId>::const_reverse_iterator rbegin() const
    {
        AssertRelease(FController != nullptr);
        return FController->rbegin();
    }

    std::set<EntityId>::iterator end()
    {
        AssertRelease(FController != nullptr);
        return FController->end();
    }
    std::set<EntityId>::const_iterator end() const
    {
        AssertRelease(FController != nullptr);
        return FController->end();
    }
    std::set<EntityId>::reverse_iterator rend()
    {
        AssertRelease(FController != nullptr);
        return FController->rend();
    }
    std::set<EntityId>::const_reverse_iterator rend() const
    {
        AssertRelease(FController != nullptr);
        return FController->rend();
    }

private:
    ModuleController<T>* FController;
};
} // namespace ECSEngine

#include "ModuleAccessor.inl"