#include "stdafx.h"

#include "GameplaySpecificLoader.h"

#include "ColonyGameRulesEditor.h"
#include "ColonyWorlds.h"
#include "GameScenarioUpdater.h"
#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
#include "ECSGameplay_Common/ScenarioFactory.h"
#include "GameplayFeedbackDrawer.h"
#include "GameplayRulesManager.h"
#include "ImGuiTools/GameRulesEditor.h"
#include "RenderingCore/FeedbackDrawerRegistry.h"

CEREAL_REGISTER_TYPE(ECSEngine::ECSGameplaySpecificLoader);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ILoader, ECSEngine::ECSGameplaySpecificLoader);
namespace ECSEngine
{

namespace
{
ECSEngine::IScenarioUpdater* CreateColonyScenario(const std::string& scenarioFile, bool forceReload)
{
    ECSEngine::GameScenarioUpdater* scene = new ECSEngine::GameScenarioUpdater();
    scene->SetScenario(scenarioFile, forceReload);
    return scene;
}
}

bool ECSGameplaySpecificLoader::VirtualInitialise()
{
    ILoader::VirtualInitialise();

    CreateColonyWorlds();
    SetScenarioFactory(&CreateColonyScenario);
    ImGUITools::SetGameRulesEditorDrawer(&ImGUITools::DrawColonyGameRulesEditor);

    GameplayFeedbackDrawer::CreateIFP();
    GameplayFeedbackDrawer::Instance().Initialise();
    SetFeedbackDrawer(&GameplayFeedbackDrawer::Instance());

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

    SetFeedbackDrawer(nullptr);
    if (GameplayFeedbackDrawer::HasInstance())
    {
        GameplayFeedbackDrawer::Instance().Shutdown();
        GameplayFeedbackDrawer::Destroy();
    }

    GameplayRulesManager::Destroy();
}

} // namespace ECSEngine
