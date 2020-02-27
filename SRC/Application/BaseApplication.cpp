#include "stdafx.h"

#include "BaseApplication.h"

#include "IGameplayUpdater.h"
#include "ILoader.h"
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
        FGameplayUpdater->StartUpdate();
        FGameplayUpdater->Update();
        FGameplayUpdater->Render();
        FGameplayUpdater->EndUpdate();
    }
}

void BaseApplicationLayer::SetGameplayUpdater_StealOwnership(IGameplayUpdater* parGameplayUpdater)
{
    FGameplayUpdater.reset(parGameplayUpdater);
}

} // namespace ECSEngine