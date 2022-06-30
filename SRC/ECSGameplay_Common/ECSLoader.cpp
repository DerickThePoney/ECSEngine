#include "stdafx.h"

#include "ECSLoader.h"

#include "Common/GenericMessageManager.h"
#include "Common/RandomGenerator.h"
#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
#include "ECSCore/AdjustableDebugParameters.h"
#include "ECSCore/EntityTemplate.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleTemplate.h"
#include "ECSCore/WorldManager.h"
#include "NavMeshPathfindingManager.h"
#include "WorldDeclaration.h"

namespace ECSEngine
{

bool ECSLoader::VirtualInitialise()
{
    ILoader::VirtualInitialise();

    CreateAdjustables();

    GenericMessageManager::CreateIFP();

    RandomNumbers::InitRandomNumberGenerator(772);

    ModuleParameters::InitParameterIdentifiersTraits();

    WorldManager::CreateIFP();
    AssertRelease(WorldManager::HasInstance());
    WorldManager& worldManagerInstance = WorldManager::Instance();
    worldManagerInstance.Init();
    CreateWorlds();

    EntityTemplateManager::CreateIFP();
    AssertRelease(EntityTemplateManager::HasInstance());
    {
        Resource r(FEntityTemplatesFile);
        auto handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&r);
        ResourceBuffer buff = handle->GetResourceBuffer();
        std::istream istr(&buff, std::istream::in);
        cereal::JSONInputArchive archive(istr);
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

    GenericMessageManager::Destroy();

    DestroyAdjustables();
}

} // namespace ECSEngine

CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ILoader, ECSEngine::ECSLoader);
