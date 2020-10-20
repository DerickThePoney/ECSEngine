#include "stdafx.h"

#include "ECSLoader.h"

#include "Common/RandomGenerator.h"
#include "Common/ResourceCache.h"
#include "ECSCore/EntityTemplate.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/WorldManager.h"
#include "WorldDeclaration.h"

namespace ECSEngine
{

bool ECSLoader::VirtualInitialise()
{
    ILoader::VirtualInitialise();

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

    return true;
}

void ECSLoader::VirtualShutdown()
{
    ILoader::VirtualShutdown();

    WorldManager::Destroy();
    EntityTemplateManagerMethods::Cleanup();
    EntityTemplateManager::Destroy();
    ECSEngine::ModuleParameters::DestroyParameterIdentifiersTraits();

    RandomNumbers::DestroyRandomNumberGenerator();
}

} // namespace ECSEngine

CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ILoader, ECSEngine::ECSLoader);