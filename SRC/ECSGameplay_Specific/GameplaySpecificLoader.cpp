#include "stdafx.h"

#include "GameplaySpecificLoader.h"

#include "Common/ResourceCache.h"
#include "GameplayRulesManager.h"

CEREAL_REGISTER_TYPE(ECSEngine::ECSGameplaySpecificLoader);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ILoader, ECSEngine::ECSGameplaySpecificLoader);
namespace ECSEngine
{

bool ECSGameplaySpecificLoader::VirtualInitialise()
{
    ILoader::VirtualInitialise();

    GameplayRulesManager::CreateIFP();

    {
        std::ifstream ifstr(GlobalResourceCache::Instance().FCache->GetBasePath() + FGameplayRulesFile);

        if (ifstr.good())
        {
            cereal::JSONInputArchive archive(ifstr);
            archive(NAMEDPROPERTY("GameplayRules", GameplayRulesManager::Instance()));
        }
    }

    return true;
}

void ECSGameplaySpecificLoader::VirtualShutdown()
{
    ILoader::VirtualShutdown();

    GameplayRulesManager::Destroy();
}

} // namespace ECSEngine
