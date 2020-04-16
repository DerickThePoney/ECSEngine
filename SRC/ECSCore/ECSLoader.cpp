#include "stdafx.h"

#include "ECSLoader.h"

#include "Common/ResourceCache.h"
#include "EntityTemplate.h"
#include "EntityTemplateManager.h"
#include "ModuleParameters.h"
#include "WorldManager.h"

namespace ECSEngine
{

bool ECSLoader::VirtualInitialise()
{
    ILoader::VirtualInitialise();

    ECSEngine::ModuleParameters::InitParameterIdentifiersTraits();

    WorldManager::CreateIFP();
    AssertRelease(WorldManager::HasInstance());
    WorldManager& worldManagerInstance = WorldManager::Instance();
    worldManagerInstance.Init();

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
    EntityTemplateManager::Destroy();
    ECSEngine::ModuleParameters::DestroyParameterIdentifiersTraits();
}

} // namespace ECSEngine

CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ILoader, ECSEngine::ECSLoader);