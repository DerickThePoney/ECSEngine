#include "stdafx.h"

#include "BuildingSelectionPanelController.h"

#include "ECSCore/EntityTemplate.h"
#include "ECSCore/ScopedModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/SelectionManager.h"
#include "RecipeProductionModule.h"
#include "ResourceStorageModule.h"
#include "UICore/RMLUIManager.h"
#include "UICore/RML_includes.h"
#include "UICore/RmlDataModelWrapper.h"
#include "UIResourceView.h"

namespace ECSEngine
{
namespace UI
{
using AccessHelpers = ScopedModuleAccessor<MC<ResourceStorageModule, EEntityWorlds::BUILDINGS>, MC<RecipeProductionModule, EEntityWorlds::BUILDINGS>>;

BuildingSelectionPanelController::BuildingSelectionPanelController()
{
}

BuildingSelectionPanelController::~BuildingSelectionPanelController()
{
}

void BuildingSelectionPanelController::VirtualInit()
{
    UIController::VirtualInit();

    Rml::DataModelConstructor ctr2 = RmlUiManager::Instance().CreateDataModel("buildingResourcesModel");

    if (auto handle = ctr2.RegisterStruct<UIResourceView>())
    {
        handle.RegisterMember("name", &UIResourceView::ResourceName);
        handle.RegisterMember("quantity", &UIResourceView::Quantity);
    }

    ctr2.RegisterArray<std::vector<UIResourceView>>();

    ctr2.Bind("buildingResourcesModel", &FResourcesInCurrentBuilding);

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
        FResourceToIndex.clear();
        FResourcesInCurrentBuilding.clear();
    }

    AccessHelpers accessHelpers;
    const ResourceStorageModule* storageModule = accessHelpers.GetModule<ResourceStorageModule>(FCurrentIdSelected);

    if (storageModule != nullptr)
    {
        auto storageSlots = storageModule->Resources();
        foreachitemconst(slot, storageSlots)
        {
            auto itFind = FResourceToIndex.find(slot.first);
            if (itFind == FResourceToIndex.end())
            {
                FResourceToIndex.insert_or_assign(slot.first, (u32)FResourcesInCurrentBuilding.size());
                FResourcesInCurrentBuilding.emplace_back(UIResourceView{ slot.first, GameResource::GetName(slot.first), (int)slot.second });
            }
            else
            {
                AlwaysCheckedAssert(FResourcesInCurrentBuilding[itFind->second].Resource == slot.first);
                FResourcesInCurrentBuilding[itFind->second].Quantity = (int)slot.second;
            }
        }
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

} // namespace UI
} // namespace ECSEngine