#include "stdafx.h"

#include "BaseApplication.h"

#include "Common/ILoader.h"
#include "IGameplayUpdater.h"
namespace ECSEngine
{

BaseApplicationLayer::BaseApplicationLayer()
{
}

BaseApplicationLayer::~BaseApplicationLayer()
{
}

void BaseApplicationLayer::Initialise()
{
    foreachitem(loader, FLoaders)
    {
        AssertRelease(loader != nullptr);
        bool res = loader->Initialise();
        AssertRelease(res);
    }

    AssertRelease(FGameplayUpdater != nullptr);
    FGameplayUpdater->Initialise();
}

void BaseApplicationLayer::Shutdown()
{
    AssertRelease(FGameplayUpdater != nullptr);
    FGameplayUpdater->Shutdown();

    foreachitem(loader, FLoaders)
    {
        AssertRelease(loader != nullptr);
        loader->Shutdown();
    }
}

void BaseApplicationLayer::RunMainLoop()
{
    while (!FGameplayUpdater->CheckShouldFinish())
    {
        SCOPED_PROFILE_CLASS(BaseApplicationLayer, RunMainLoop);
        FGameplayUpdater->StartUpdate();
        FGameplayUpdater->GameplayUpdate();
        FGameplayUpdater->UIUpdate();
        FGameplayUpdater->DebugRender();
        FGameplayUpdater->Render();
        FGameplayUpdater->EndUpdate();
        FRAME_END;
    }
}

void BaseApplicationLayer::SetGameplayUpdater_StealOwnership(IGameplayUpdater* parGameplayUpdater)
{
    FGameplayUpdater.reset(parGameplayUpdater);
}

} // namespace ECSEngine
