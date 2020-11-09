#include "stdafx.h"

#include "ECSLoader.h"

#include "Common/RandomGenerator.h"
#include "Common/ResourceCache.h"
#include "ECSCore/AdjustableDebugParameters.h"
#include "ECSCore/EntityTemplate.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/WorldManager.h"
#include "PathfindingManager.h"
#include "WorldDeclaration.h"

namespace ECSEngine
{

bool ECSLoader::VirtualInitialise()
{
    ILoader::VirtualInitialise();

    CreateAdjustables();

    RandomNumbers::InitRandomNumberGenerator(772);

    ModuleParameters::InitParameterIdentifiersTraits();
    ModuleTemplates::InitModuleTemplateFactories();

    WorldManager::CreateIFP();
    AssertRelease(WorldManager::HasInstance());
    WorldManager& worldManagerInstance = WorldManager::Instance();
    worldManagerInstance.Init();
    CreateWorlds();

    EntityTemplateManager::CreateIFP();
    AssertRelease(EntityTemplateManager::HasInstance());
    AssertRelease(EntityTemplateManager::HasInstance());
    {
        std::ifstream ifstr(GlobalResourceCache::Instance().FCache->GetBasePath() + FEntityTemplatesFile);

        cereal::JSONInputArchive archive(ifstr);
        archive(NAMEDPROPERTY("EntityTemplatesList", EntityTemplateManager::Instance()));
    }

    Pathfinding::CreatePathfinder();

    return true;
}

void ECSLoader::VirtualShutdown()
{
    ILoader::VirtualShutdown();

    Pathfinding::DestroyPathfinder();

    EntityTemplateManagerMethods::Cleanup();
    EntityTemplateManager::Destroy();
    WorldManager::Destroy();
    ECSEngine::ModuleParameters::DestroyParameterIdentifiersTraits();

    RandomNumbers::DestroyRandomNumberGenerator();

    DestroyAdjustables();
}

} // namespace ECSEngine

CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ILoader, ECSEngine::ECSLoader);