#include "stdafx.h"

#include "GameplaySpecificLoader.h"

#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
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
        Resource r(FGameplayRulesFile);
        auto handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&r);
        AlwaysCheckedAssert(handle != nullptr);
        if (handle != nullptr)
        {
            ResourceBuffer buff = handle->GetResourceBuffer();
            std::istream istr(&buff, std::istream::in);

            cereal::JSONInputArchive archive(istr);
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
