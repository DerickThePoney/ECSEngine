#pragma once
#include "EntityId.h"
#include "WorldIds.h"

namespace ECSEngine
{
class EntityWorld;
template<typename T>
class ModuleController;

template<typename T>
class ModuleAccessor
{
public:
    ModuleAccessor(const Worlds::Type parWorld = Worlds::STANDARD);
    ModuleAccessor(EntityWorld* world);

    T* operator[](const EntityId& parId);
    const T* operator[](const EntityId& parId) const;

    using iterator = typename ModuleController<T>::iterator;
    using const_iterator = typename ModuleController<T>::const_iterator;
    using reverse_iterator = typename ModuleController<T>::reverse_iterator;
    using const_reverse_iterator = typename ModuleController<T>::const_reverse_iterator;

    iterator begin()
    {
        AssertRelease(FController != nullptr);
        return FController->begin();
    }
    const_iterator cbegin() const
    {
        AssertRelease(FController != nullptr);
        return FController->cbegin();
    }
    reverse_iterator rbegin()
    {
        AssertRelease(FController != nullptr);
        return FController->rbegin();
    }
    const_reverse_iterator crbegin() const
    {
        AssertRelease(FController != nullptr);
        return FController->crbegin();
    }

    iterator end()
    {
        AssertRelease(FController != nullptr);
        return FController->end();
    }
    const_iterator cend() const
    {
        AssertRelease(FController != nullptr);
        return FController->cend();
    }
    reverse_iterator rend()
    {
        AssertRelease(FController != nullptr);
        return FController->rend();
    }
    const_reverse_iterator crend() const
    {
        AssertRelease(FController != nullptr);
        return FController->crend();
    }

    u32 size() const { return FController->GetSize(); }

private:
    ModuleController<T>* FController;
};

} // namespace ECSEngine

#include "ModuleAccessor.inl"