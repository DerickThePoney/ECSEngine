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

    typename ModuleController<T>::iterator begin()
    {
        AssertRelease(FController != nullptr);
        return FController->begin();
    }
    /*typename ModuleController<T>::const_iterator begin() const
    {
        AssertRelease(FController != nullptr);
        return FController->begin();
    }*/
    typename ModuleController<T>::reverse_iterator rbegin()
    {
        AssertRelease(FController != nullptr);
        return FController->rbegin();
    }
    /*typename ModuleController<T>::const_reverse_iterator rbegin() const
    {
        AssertRelease(FController != nullptr);
        return FController->rbegin();
    }*/

    typename ModuleController<T>::iterator end()
    {
        AssertRelease(FController != nullptr);
        return FController->end();
    }
    /*typename ModuleController<T>::const_iterator end() const
    {
        AssertRelease(FController != nullptr);
        return FController->end();
    }*/
    typename ModuleController<T>::reverse_iterator rend()
    {
        AssertRelease(FController != nullptr);
        return FController->rend();
    }
    /*typename ModuleController<T>::const_reverse_iterator rend() const
    {
        AssertRelease(FController != nullptr);
        return FController->rend();
    }*/

private:
    ModuleController<T>* FController;
};
} // namespace ECSEngine

#include "ModuleAccessor.inl"