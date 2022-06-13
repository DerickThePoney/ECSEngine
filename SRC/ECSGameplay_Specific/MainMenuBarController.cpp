#include "stdafx.h"

#include "MainMenuBarController.h"

#include "ColonyModule.h"
#include "ColonyPeonsManagementModule.h"
#include "Common/Logger.h"
#include "Common/TimeManager.h"
#include "ECSCore/ScopedModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "EnergyProducerModule.h"
#include "EnergySystem.h"
#include "HousingPlaceModule.h"
#include "PeonFeedingTimeModule.h"
#include "PeonSpawnModule.h"
#include "ResourceStorageModule.h"
#include "UICore/RMLUIManager.h"
#include "UICore/RML_includes.h"

namespace ECSEngine
{
namespace UI
{

using AccessHelpers = ScopedModuleAccessor<MC<ColonyModule, EEntityWorlds::COLONY>,
      MC<ResourceStorageModule, EEntityWorlds::COLONY>,
      MC<PeonSpawnModule, EEntityWorlds::COLONY>,
      MC<ColonyPeonsManagementModule, EEntityWorlds::COLONY>,
      MC<PeonFeedingTimeModule, EEntityWorlds::COLONY>,
      MC<HousingPlaceModule, EEntityWorlds::BUILDINGS>,
      MC<EnergyProducerModule, EEntityWorlds::BUILDINGS>>;

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
    UIController::VirtualInit();

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
    UIController::VirtualUpdate();
    if (!HandleVisibility())
        return;

    AccessHelpers accessHelpers;

    const EntityId colonyId = EntityId((u32)EEntityWorlds::COLONY, 0);

    const ColonyModule* colonyModule = accessHelpers.GetModule<ColonyModule>(colonyId);
    if (colonyModule == nullptr)
        return;

    const ResourceStorageModule* resourceStorage = accessHelpers.GetModule<ResourceStorageModule>(colonyId);
    AssertRelease(resourceStorage != nullptr);

    const ColonyPeonsManagementModule* peonManagerModule = accessHelpers.GetModule<ColonyPeonsManagementModule>(colonyId);
    AssertRelease(peonManagerModule != nullptr);

    PeonSpawnModule* peonSpawnModule = accessHelpers.GetModule<PeonSpawnModule>(colonyId);
    AssertRelease(peonSpawnModule != nullptr);

    const PeonFeedingTimeModule* peonFeedingTimeModule = accessHelpers.GetModule<PeonFeedingTimeModule>(colonyId);

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
    UIController::VirtualDestroy();
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