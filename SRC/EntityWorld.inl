#pragma once
#include "ModuleController.h"
#include "ModuleId.h"

namespace ECSEngine
{
template<typename T>
void EntityWorld::AddController()
{
    constexpr u32 moduleId = ModuleTraits<T>::GetModuleId();
    AssertRelease(moduleId < FSize);
    AssertRelease(FControllers[moduleId] != nullptr);
    FControllers[moduleId] = new ModuleController<T>();
}

template<typename T>
IModuleController* EntityWorld::GetControllerIFP()
{
    constexpr u32 moduleId = ModuleTraits<T>::GetModuleId();
    AssertRelease(moduleId < FSize);
    return FControllers[moduleId];
}

} // namespace ECSEngine