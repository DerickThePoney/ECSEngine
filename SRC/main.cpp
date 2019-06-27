#include "stdafx.h"

#include "EntityId.h"
#include "EntityTemplateManager.h"
#include "EntityWorld.h"
#include "ModuleAccessor.h"
#include "ModuleController.h"
#include "ModuleId.h"
#include "MovementSystem.h"
#include "PositionModule.h"
#include "WorldManager.h"

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

    ECSEngine::WorldManager::CreateIFP();
    AssertRelease(ECSEngine::WorldManager::HasInstance());
    ECSEngine::WorldManager& worldManagerInstance = ECSEngine::WorldManager::Instance();
    worldManagerInstance.Init();

    ECSEngine::EntityWorld& world = worldManagerInstance.GetWorld(ECSEngine::Worlds::STANDARD);

    ECSEngine::EntityTemplateManager::CreateIFP();
    AssertRelease(ECSEngine::EntityTemplateManager::HasInstance());
    ECSEngine::EntityTemplate* newTemplate = ECSEngine::EntityTemplateManager::Instance().CreateNewEntityTemplate();
    newTemplate->SetHasModule<ECSEngine::PositionModule>();

    ECSEngine::EntityId unitID = world.CreateEntityFromTemplateReturnEntityId(newTemplate);
    ECSEngine::EntityId unitID2 = world.CreateEntityFromTemplateReturnEntityId(newTemplate);

    dynamic_cast<ECSEngine::ModuleController<ECSEngine::PositionModule>*>(world.GetControllerIFP<ECSEngine::PositionModule>())->Lock();
    ECSEngine::ModuleAccessor<ECSEngine::PositionModule> moduleAccessor;

    ECSEngine::PositionModule* positionModule = moduleAccessor[unitID];
    const ECSEngine::PositionModule* positionModuleConst = moduleAccessor[unitID];

    AssertRelease(positionModule != nullptr && positionModule == positionModuleConst);
    foreachitem(positionMod, moduleAccessor) { std::cout << "Position module " << &positionMod << "\t" << glm::to_string(positionMod.GetPosition3D()) << std::endl; }
    foreachitemconst(positionMod, moduleAccessor) { std::cout << "Position module " << &positionMod << "\t" << glm::to_string(positionMod.GetPosition3D()) << std::endl; }
    reverseforeachitem(positionMod, moduleAccessor) { std::cout << "Position module " << &positionMod << "\t" << glm::to_string(positionMod.GetPosition3D()) << std::endl; }
    reverseforeachitemconst(positionMod, moduleAccessor) { std::cout << "Position module " << &positionMod << "\t" << glm::to_string(positionMod.GetPosition3D()) << std::endl; }

    dynamic_cast<ECSEngine::ModuleController<ECSEngine::PositionModule>*>(world.GetControllerIFP<ECSEngine::PositionModule>())->Unlock();

    ECSEngine::EntityId unitID3 = world.CreateEntityFromTemplateReturnEntityId(newTemplate);
    ECSEngine::EntityId unitID4 = world.CreateEntityFromTemplateReturnEntityId(newTemplate);
    ECSEngine::EntityId unitID5 = world.CreateEntityFromTemplateReturnEntityId(newTemplate);
    ECSEngine::EntityId unitID6 = world.CreateEntityFromTemplateReturnEntityId(newTemplate);

    ECSEngine::MovementSystem movementSystem;
    movementSystem.Init();

    for (int i = 0; i < 1000000; ++i)
    {
        movementSystem.Update();
    }

    world.DestroyEntity(unitID);
    world.DestroyEntity(unitID2);
    world.DestroyEntity(unitID3);
    world.DestroyEntity(unitID4);
    world.DestroyEntity(unitID5);
    world.DestroyEntity(unitID6);

    worldManagerInstance.Destroy();

    return 0;
}