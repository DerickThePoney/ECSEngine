#include "WorldManager.h"

namespace ECSEngine
{
template<typename T>
const T* Module::Template()
{
    EntityWorld& world = WorldManager::Instance().GetWorld(FUnitId.GetWorld());
    const EntityTemplate* temp = world.GetTemplateForEntity(FUnitId);
    AssertRelease(temp != nullptr);

    const ModuleTemplate* modTemp = temp->GetModuleTemplate(GetModuleId());
    AssertRelease(modTemp != nullptr);

    return static_cast<const T*>(modTemp);
}
} // namespace ECSEngine
