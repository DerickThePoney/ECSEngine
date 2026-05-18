#include "stdafx.h"

#include "BuildMenuController.h"

#include "BuildingCostManager.h"
#include "GameplayRulesManager.h"
#include "MousePolicyManager.h"
#include "PlaceBuildingMousePolicy.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "UICore/RMLUIManager.h"
#include "UICore/RML_includes.h"

namespace ECSEngine
{
namespace UI
{

struct BuildMenuModel
{
    BuildMenuModel(BuildMenuController* parController);
    void OnBuildIconClicked(Rml::DataModelHandle parHandle, Rml::Event& parEvent, const Rml::VariantList& parArgs);

    BuildMenuController* FController = nullptr;
};

BuildMenuModel::BuildMenuModel(BuildMenuController* parController)
    : FController(parController)
{
}

void BuildMenuModel::OnBuildIconClicked(Rml::DataModelHandle parHandle, Rml::Event& parEvent, const Rml::VariantList& parArgs)
{
    u32 val = static_cast<u32>(parArgs[0].Get<double>());
    FController->SetIsPlacingBuilding(val);
}

static std::unique_ptr<BuildMenuModel> sBuildMenuModel;

BuildMenuController::BuildMenuController()
{
}

void BuildMenuController::VirtualInit()
{
    UIController::VirtualInit();

    sBuildMenuModel.reset(new BuildMenuModel(this));

    Rml::DataModelConstructor ctr2 = RmlUiManager::Instance().CreateDataModel("build-menu-model");
    ctr2.BindEventCallback("buildiconclicked", &BuildMenuModel::OnBuildIconClicked, sBuildMenuModel.get());
    FDataModelWrapper = RmlDataModelWrapperFactory::CreateDataModelWrapper(ctr2.GetModelHandle());
    FDocument = RmlUiManager::Instance().LoadDocument("UI\\BuildMenu\\BuildMenu.rml");
    AssertRelease(FDocument != nullptr);
    FDocument->GetElementById("title")->SetInnerRML(FDocument->GetTitle());

    FillWindow();
}

void BuildMenuController::VirtualUpdate()
{
    UIController::VirtualUpdate();

    if (!HandleVisibility())
        return;
}

void BuildMenuController::VirtualDestroy()
{
    UIController::VirtualDestroy();

    if (FDocument != nullptr)
    {
        RmlUiManager::Instance().UnloadDocument(FDocument);
    }

    RmlUiManager::Instance().RemoveDataModel("build-menu-model");

    sBuildMenuModel.reset(nullptr);
}

bool BuildMenuController::HandleVisibility()
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

void BuildMenuController::SetIsPlacingBuilding(const u32 parBuildingIndex)
{
    auto& buildCostDesc = GameplayRulesManager::Instance().FBuildingCostManager.GetBuildingCostDescriptors();
    AssertRelease(parBuildingIndex < buildCostDesc.size());

    auto& costRule = buildCostDesc[parBuildingIndex];

    PlaceBuildingMousePolicy* mousePolicy = MousePolicyManager::Instance().GetMousePolicy<PlaceBuildingMousePolicy>(MousePolicyType::PLACE_BUILDING);
    if (mousePolicy->IsActivated())
        mousePolicy->Deactivate();
    mousePolicy->SetupMousePolicy(costRule.BuildingTemplateName());
    mousePolicy->Activate();
}

void BuildMenuController::FillWindow()
{
    // AddTabs
    Rml::ElementList tabs;
    FDocument->GetElementsByTagName(tabs, "tabs");
    AssertRelease(tabs.size() == 1);
    AssertRelease(tabs[0] != nullptr);

    Rml::ElementList panels;
    FDocument->GetElementsByTagName(panels, "panels");
    AssertRelease(panels.size() == 1);
    AssertRelease(panels[0] != nullptr);

    forrange(i, 0, BuildingCategory::LENGTH)
    {
        Rml::ElementPtr newTab = FDocument->CreateElement("tab");
        newTab->SetInnerRML(BuildingCategory::AsString((BuildingCategory::Type)i));
        tabs[0]->AppendChild(std::move(newTab));

        Rml::ElementPtr newPanel = FDocument->CreateElement("panel");
        newPanel->SetId(std::format("{}-panel", BuildingCategory::AsString((BuildingCategory::Type)i)).c_str());
        panels[0]->AppendChild(std::move(newPanel));
    }

    auto& buildCostDesc = GameplayRulesManager::Instance().FBuildingCostManager.GetBuildingCostDescriptors();
    forrange(i, 0, buildCostDesc.size())
    {
        auto& costRule = buildCostDesc[i];

        Rml::Element* tabToUse = FDocument->GetElementById(std::format("{}-panel", BuildingCategory::AsString(costRule.BuildingType())).c_str());
        AssertRelease(tabToUse != nullptr);
        Rml::ElementPtr newBuilding = FDocument->CreateElement("buildicon");
        newBuilding->SetId(costRule.BuildingTemplateName().c_str());
        newBuilding->SetInnerRML(costRule.BuildingTemplateName().c_str());
        newBuilding->SetAttribute("data-event-click", std::format("buildiconclicked({})", i).c_str());
        tabToUse->AppendChild(std::move(newBuilding));
    }
}

} // namespace UI
} // namespace ECSEngine
