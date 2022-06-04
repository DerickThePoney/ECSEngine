#include "stdafx.h"

#include "MainMenuBarController.h"

#include "Common/Logger.h"
#include "Common/TimeManager.h"
#include "ECSCore/WorldIds.h"
#include "EnergySystem.h"
#include "UICore/RMLUIManager.h"
#include "UICore/RML_includes.h"

namespace ECSEngine
{
namespace UI
{

void ClickedTestData(Rml::DataModelHandle model_handle, Rml::Event& ev, const Rml::VariantList& ag)
{
    std::cout << "oh yeah!!! " << ev.GetTargetElement()->GetInnerRML() << "\n";
}

MainMenuBarController::MainMenuBarController()
{
}

MainMenuBarController::~MainMenuBarController()
{
}

void MainMenuBarController::VirtualInit()
{
    UIControllerWithModuleAccessors::VirtualInit();

    Rml::DataModelConstructor ctr = RmlUiManager::Instance().CreateDataModel("colony-model");
    AssertRelease((bool)ctr);
    ctr.Bind("peons", &FModel.TotalPeons);
    ctr.Bind("idle", &FModel.IdlePeons);
    ctr.Bind("influence", &FModel.Influence);
    ctr.Bind("energy", &FModel.Energy);
    ctr.Bind("remaining_feeding_time", &FModel.RemainingFeedingTime);

    FDataModelWrapper = RmlDataModelWrapperFactory::CreateDataModelWrapper(ctr.GetModelHandle());

    FDocument = RmlUiManager::Instance().LoadDocument("UI\\MainMenuBar\\MainMenuBar.rml");
}

void MainMenuBarController::VirtualUpdate()
{
    UIControllerWithModuleAccessors::VirtualUpdate();
    if (!HandleVisibility())
        return;

    const EntityId colonyId = EntityId((u32)EEntityWorlds::COLONY, 0);

    const ColonyModule* colonyModule = GetModule<ColonyModule>(colonyId);
    if (colonyModule == nullptr)
        return;

    const ResourceStorageModule* resourceStorage = GetModule<ResourceStorageModule>(colonyId);
    AssertRelease(resourceStorage != nullptr);

    const ColonyPeonsManagementModule* peonManagerModule = GetModule<ColonyPeonsManagementModule>(colonyId);
    AssertRelease(peonManagerModule != nullptr);

    PeonSpawnModule* peonSpawnModule = GetModule<PeonSpawnModule>(colonyId);
    AssertRelease(peonSpawnModule != nullptr);

    const PeonFeedingTimeModule* peonFeedingTimeModule = GetModule<PeonFeedingTimeModule>(colonyId);

    FModel.TotalPeons = peonManagerModule->PeonsInColony();
    FModel.IdlePeons = peonManagerModule->IdlePeons().size();
    FModel.RemainingFeedingTime = peonFeedingTimeModule->RemainingTimeBeforeNextFeed();
    FModel.Influence = resourceStorage->GetResourceQuantity(GameResource::INFLUENCE);
    FDataModelWrapper->DirtyVariable("peons");
    FDataModelWrapper->DirtyVariable("idle");
    FDataModelWrapper->DirtyVariable("influence");
    FDataModelWrapper->DirtyVariable("remaining_feeding_time");

    UpdateEnergy();

    const float progress = peonFeedingTimeModule->RemainTimeBeforeNextFeedAsRatio();
    Rml::Element* progressBar = FDocument->GetElementById("progress");
    AssertRelease(progressBar != nullptr);
    progressBar->SetAttribute("value", progress);
}

void MainMenuBarController::VirtualDestroy()
{
    UIControllerWithModuleAccessors::VirtualDestroy();
    if (FDocument != nullptr)
    {
        RmlUiManager::Instance().UnloadDocument(FDocument);
    }
    RmlUiManager::Instance().RemoveDataModel("colony-model");
}

bool MainMenuBarController::HandleVisibility()
{
    const bool visible = FDocument->IsVisible();
    if (FShow && !visible)
    {
        FDocument->Show();
    }
    else if (!FShow && visible)
    {
        FDocument->Hide();
    }
    return FShow;
}

void MainMenuBarController::UpdateEnergy()
{
    FModel.Energy = EnergySystem::Instance().TotalEnergy();
    FDataModelWrapper->DirtyVariable("energy");
}

} // namespace UI
} // namespace ECSEngine