#include "stdafx.h"

#include "BuildingSelectionPanelController.h"

#include "ECSCore/EntityTemplate.h"
#include "ECSCore/ScopedModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/SelectionManager.h"
#include "UICore/RMLUIManager.h"
#include "UICore/RML_includes.h"
#include "UICore/RmlDataModelWrapper.h"

namespace ECSEngine
{
namespace UI
{
using AccessHelpers = ScopedModuleAccessor<MC<StorageSlotModule, EEntityWorlds::BUILDINGS>, MC<RecipeProductionModule, EEntityWorlds::BUILDINGS>>;

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

    /*if (auto handle = ctr2.RegisterStruct<ResourceView>())
    {
        handle.RegisterMember("name", &ResourceView::ResourceName);
        handle.RegisterMember("quantity", &ResourceView::Quantity);
    }

    ctr2.RegisterArray<decltype(FResourceView)>();

    ctr2.Bind("resources", &FResourceView);*/

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