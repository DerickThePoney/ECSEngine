#include "stdafx.h"

#include "BuildingSelectionPanelController.h"

#include "ECSCore/EntityFactory.h"
#include "ECSCore/EntityTemplate.h"
#include "ECSCore/ScopedModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/SelectionManager.h"
#include "EnergyProducerModule.h"
#include "EnergySystem.h"
#include "RecipeHelpers.h"
#include "RecipeProductionModule.h"
#include "ResourceStorageModule.h"
#include "StorageSlotModule.h"
#include "UICore/RMLUIManager.h"
#include "UICore/RML_includes.h"
#include "UICore/RmlDataModelWrapper.h"
#include "UIResourceView.h"

namespace ECSEngine
{
namespace UI
{
using AccessHelpers = ScopedModuleAccessor<MC<ResourceStorageModule, EEntityWorlds::BUILDINGS>,
      MC<RecipeProductionModule, EEntityWorlds::BUILDINGS>,
      MC<EnergyProducerModule, EEntityWorlds::BUILDINGS>,
      MC<StorageSlotModule, EEntityWorlds::BUILDINGS>>;

struct BuildingSelectionPanelCallbackListener
{
    BuildingSelectionPanelCallbackListener(BuildingSelectionPanelController* parController);
    void OnDeleteButtonClicked(Rml::DataModelHandle parHandle, Rml::Event& parEvent, const Rml::VariantList& parArgs);

    BuildingSelectionPanelController* FController = nullptr;
};

BuildingSelectionPanelCallbackListener::BuildingSelectionPanelCallbackListener(BuildingSelectionPanelController* parController)
    : FController(parController)
{
}

void BuildingSelectionPanelCallbackListener::OnDeleteButtonClicked(Rml::DataModelHandle parHandle, Rml::Event& parEvent, const Rml::VariantList& parArgs)
{
    FController->OnDeleteButtonClicked();
}

static std::unique_ptr<BuildingSelectionPanelCallbackListener> sBuildingSelectionPanelCallbackListener;

BuildingSelectionPanelController::BuildingSelectionPanelController()
{
}

BuildingSelectionPanelController::~BuildingSelectionPanelController()
{
}

void BuildingSelectionPanelController::VirtualInit()
{
    UIController::VirtualInit();

    sBuildingSelectionPanelCallbackListener.reset(new BuildingSelectionPanelCallbackListener(this));

    Rml::DataModelConstructor ctr2 = RmlUiManager::Instance().CreateDataModel("buildingResourcesModel");

    if (auto handle = ctr2.RegisterStruct<UIStorageSlotsView>())
    {
        handle.RegisterMember("resource", &UIStorageSlotsView::ResourceName);
        handle.RegisterMember("quantity", &UIStorageSlotsView::Quantity);
        handle.RegisterMember("isFree", &UIStorageSlotsView::IsFree);
    }

    ctr2.RegisterArray<UIResourceArray>();
    ctr2.RegisterArray<std::vector<UIStorageSlotsView>>();
    if (auto handle = ctr2.RegisterStruct<BuildingSelectionPanelDataView>())
    {
        handle.RegisterMember("has_resources", &BuildingSelectionPanelDataView::HasResources);
        handle.RegisterMember("has_recipe", &BuildingSelectionPanelDataView::HasRecipe);
        handle.RegisterMember("recipeInput", &BuildingSelectionPanelDataView::FRecipeInputs);
        handle.RegisterMember("recipeOutput", &BuildingSelectionPanelDataView::FRecipeOutputs);
        handle.RegisterMember("recipeDuration", &BuildingSelectionPanelDataView::FRecipeDuration);
        handle.RegisterMember("recipeBaseDuration", &BuildingSelectionPanelDataView::FRecipeBaseDuration);
        handle.RegisterMember("efficiency", &BuildingSelectionPanelDataView::FEfficiency);
        handle.RegisterMember("produces_energy", &BuildingSelectionPanelDataView::ProducesEnergy);
        handle.RegisterMember("energy_produced", &BuildingSelectionPanelDataView::EnergyProduced);
        handle.RegisterMember("resources", &BuildingSelectionPanelDataView::FResourcesInCurrentBuilding);
        handle.RegisterMember("has_storage_slots", &BuildingSelectionPanelDataView::HasStorageSlots);
        handle.RegisterMember("storage_slots", &BuildingSelectionPanelDataView::StorageSlots);
    }

    ctr2.Bind("buildingResourcesModel", &FDataView);
    ctr2.BindEventCallback("deletebuilding", &BuildingSelectionPanelCallbackListener::OnDeleteButtonClicked, sBuildingSelectionPanelCallbackListener.get());

    FDataModelWrapper = RmlDataModelWrapperFactory::CreateDataModelWrapper(ctr2.GetModelHandle());

    FDocument = RmlUiManager::Instance().LoadDocument("UI\\BuildingSelectionPanel\\BuildingSelectionPanel.rml");
    AssertRelease(FDocument != nullptr);
    FDocument->GetElementById("title")->SetInnerRML(FDocument->GetTitle());
}

void BuildingSelectionPanelController::VirtualUpdate()
{
    UIController::VirtualUpdate();

    if (!HandleVisibility())
        return;

    const EntityTemplate* buildingTemplate = WorldManager::Instance().GetTemplateForEntityId(FCurrentIdSelected);
    AssertRelease(buildingTemplate != nullptr);

    FDocument->GetElementById("title")->SetInnerRML(buildingTemplate->GetName());

    if (FPreviousIdSelected != FCurrentIdSelected)
    {
        ResetDataView();
    }

    AccessHelpers accessHelpers;
    const RecipeProductionModule* recipeProductionModule = accessHelpers.GetModule<RecipeProductionModule>(FCurrentIdSelected);
    if (recipeProductionModule != nullptr)
    {
        HandleRecipe(recipeProductionModule);
    }

    const ResourceStorageModule* storageModule = accessHelpers.GetModule<ResourceStorageModule>(FCurrentIdSelected);
    if (storageModule != nullptr)
    {
        HandleResources(storageModule);
    }

    const EnergyProducerModule* energyProducerModule = accessHelpers.GetModule<EnergyProducerModule>(FCurrentIdSelected);
    if (energyProducerModule != nullptr)
    {
        FDataView.ProducesEnergy = true;
        FDataView.EnergyProduced = energyProducerModule->ProducedEnergy();
    }

    const StorageSlotModule* storageSlotModule = accessHelpers.GetModule<StorageSlotModule>(FCurrentIdSelected);
    if (storageSlotModule != nullptr)
    {
        HandleStorageSlots(storageSlotModule);
    }

    FDataModelWrapper->DirtyVariable("buildingResourcesModel");
}

void BuildingSelectionPanelController::VirtualDestroy()
{
    UIController::VirtualDestroy();

    if (FDocument != nullptr)
    {
        RmlUiManager::Instance().UnloadDocument(FDocument);
    }

    RmlUiManager::Instance().RemoveDataModel("buildingResourcesModel");
    sBuildingSelectionPanelCallbackListener.reset(nullptr);
}

bool BuildingSelectionPanelController::HandleVisibility()
{
    FPreviousIdSelected = FCurrentIdSelected;

    auto selectedUnits = SelectionManager::Instance().SelectedUnits();

    if (selectedUnits.empty())
    {
        FCurrentIdSelected = EntityId();
    }
    else
    {
        foreachitemconst(unitId, selectedUnits)
        {
            if (unitId.GetWorld() != EEntityWorlds::BUILDINGS)
                continue;
            FCurrentIdSelected = unitId;
            break;
        }
    }

    FShow = FCurrentIdSelected.Valid();

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

void BuildingSelectionPanelController::ResetDataView()
{
    FDataView.HasResources = false;
    FDataView.FResourceToIndex.clear();
    FDataView.FResourcesInCurrentBuilding.clear();
    FDataView.HasRecipe = false;
    FDataView.FRecipeInputs.clear();
    FDataView.FRecipeOutputs.clear();
    FDataView.FRecipeDuration = 0.f;
    FDataView.ProducesEnergy = false;
    FDataView.EnergyProduced = 0.f;
    FDataView.HasStorageSlots = false;
    FDataView.StorageSlots.clear();
}

void BuildingSelectionPanelController::HandleResources(const ResourceStorageModule* parStorageModule)
{
    FDataView.HasResources = true;
    auto storageSlots = parStorageModule->Resources();
    foreachitemconst(slot, storageSlots)
    {
        auto itFind = FDataView.FResourceToIndex.find(slot.first);
        if (itFind == FDataView.FResourceToIndex.end())
        {
            FDataView.FResourceToIndex.insert_or_assign(slot.first, (u32)FDataView.FResourcesInCurrentBuilding.size());
            FDataView.FResourcesInCurrentBuilding.emplace_back(UIResourceView{ slot.first, GameResource::GetName(slot.first), (int)slot.second });
        }
        else
        {

            AlwaysCheckedAssert(FDataView.FResourcesInCurrentBuilding[itFind->second].Resource == slot.first);
            FDataView.FResourcesInCurrentBuilding[itFind->second].Quantity = (int)slot.second;
        }
    }
}

void BuildingSelectionPanelController::HandleRecipe(const RecipeProductionModule* parRecipeModule)
{
    FDataView.HasRecipe = true;

    const ProductionRecipe* recipe = parRecipeModule->GetProductionRecipe();
    FDataView.FRecipeInputs.clear();
    FDataView.FRecipeOutputs.clear();

    foreachitemconst(input, recipe->InputComponents())
    {
        FDataView.FRecipeInputs.emplace_back(UIResourceView{ input.first, GameResource::GetName(input.first), (int)input.second });
    }

    foreachitemconst(input, recipe->OutputComponents())
    {
        FDataView.FRecipeOutputs.emplace_back(UIResourceView{ input.first, GameResource::GetName(input.first), (int)input.second });
    }

    FDataView.FRecipeDuration = ProductionRecipeHelpers::ComputeCraftDuration(recipe);
    FDataView.FRecipeBaseDuration = recipe->CraftDuration();
    FDataView.FEfficiency = EnergySystem::Instance().EnergyEfficiency();

    const float progress = (recipe->CraftDuration() - parRecipeModule->ProductionTimeRemaining()) / recipe->CraftDuration();
    Rml::Element* progressBar = FDocument->GetElementById("progress");
    AssertRelease(progressBar != nullptr);
    progressBar->SetAttribute("value", progress);
}

void BuildingSelectionPanelController::HandleStorageSlots(const StorageSlotModule* storageModule)
{
    FDataView.HasStorageSlots = true;
    MemoryView<const StorageSlot> storageSlots = storageModule->StorageSlots();
    FDataView.StorageSlots.clear();
    FDataView.StorageSlots.reserve(storageSlots.size());
    foreachitemconst(slot, storageSlots)
    {
        FDataView.StorageSlots.push_back(UIStorageSlotsView{ slot.Resource, GameResource::GetName(slot.Resource), slot.Quantity, !slot.FReservedForBuilding.Valid() });
    }
}

void BuildingSelectionPanelController::OnDeleteButtonClicked()
{
    AssertRelease(FCurrentIdSelected.Valid());
    EntityFactory::MarkEntityAsDead(FCurrentIdSelected);
    FPreviousIdSelected = FCurrentIdSelected;
    ResetDataView();
}

} // namespace UI
} // namespace ECSEngine