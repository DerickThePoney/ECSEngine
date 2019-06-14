#include "stdafx.h"

#include "EntityId.h"
#include "EntityTemplateManager.h"
#include "EntityWorld.h"
#include "ModuleAccessor.h"
#include "ModuleController.h"
#include "ModuleId.h"
#include "PositionModule.h"
#include "WorldDeclaration.h"

int main(int argc, char** argv)
{
    std::cout << "EntityID size: " << sizeof(ECSEngine::EntityId) << "\n";
    std::cout << "glm::vec3 size: " << sizeof(glm::aligned_vec3) << "\n";

    ECSEngine::ModuleTraits<ECSEngine::PositionModule> b;

    constexpr u32 res = b.GetModuleId();

    ECSEngine::ModuleController<ECSEngine::PositionModule> controller;

    controller.AllocateForEntity(ECSEngine::EntityId(0, 0));

    std::cout << "PositionModule Id size: " << ECSEngine::ModuleTraits<ECSEngine::PositionModule>::GetModuleId() << "\n";

    controller.DeallocateForEntity(ECSEngine::EntityId(0, 0));

    ECSEngine::EntityWorld world(std::move(ECSEngine::CreateWorld()));

    ECSEngine::EntityTemplateManager::CreateIFP();
    AssertRelease(ECSEngine::EntityTemplateManager::HasInstance());
    ECSEngine::EntityTemplate* newTemplate = ECSEngine::EntityTemplateManager::Instance().CreateNewEntityTemplate();
    newTemplate->SetHasModule<ECSEngine::PositionModule>();

    ECSEngine::EntityId unitID = world.CreateEntityFromTemplateReturnEntityId(newTemplate);

    dynamic_cast<ECSEngine::ModuleController<ECSEngine::PositionModule>*>(world.GetControllerIFP<ECSEngine::PositionModule>())->Lock();
    ECSEngine::ModuleAccessor<ECSEngine::PositionModule> moduleAccessor(&world);

    ECSEngine::PositionModule* positionModule = moduleAccessor[unitID];
    const ECSEngine::PositionModule* positionModuleConst = moduleAccessor[unitID];

    AssertRelease(positionModule != nullptr && positionModule == positionModuleConst);
    ECSEngine::ModuleAccessor<ECSEngine::PositionModule>::const_iterator it = moduleAccessor.cbegin();
    for (; it != moduleAccessor.cend(); ++it)
    {
        std::cout << "Position module " << *it << std::endl;
    }

    dynamic_cast<ECSEngine::ModuleController<ECSEngine::PositionModule>*>(world.GetControllerIFP<ECSEngine::PositionModule>())->Unlock();

    world.DestroyEntity(unitID);

    return 0;
}