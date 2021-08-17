#include "stdafx.h"

#include "BuildMenuController.h"

#include "BuildingCostManager.h"
#include "GameplayRulesManager.h"
#include "MousePolicyManager.h"
#include "PlaceBuildingMousePolicy.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RmlUi/Core/DataModelHandle.h"
#include "UICore/RMLUIManager.h"

#include <RmlUi/Core/DataModelHandle.h>
#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Types.h>

namespace ECSEngine
{
namespace UI
{

BuildMenuController::BuildMenuController()
{
}

void BuildMenuController::VirtualInit()
{
    UIController::VirtualInit();

    Rml::DataModelConstructor ctr2 = RmlUiManager::Instance().CreateDataModel("build-menu-model");
    //    ctr2.BindEventCallback("clickedicontest", &ClickedTestData);
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

    RmlUiManager::Instance().RemoveDataModel("build-menu-model");
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
        newPanel->SetId(fmt::format("{}-panel", BuildingCategory::AsString((BuildingCategory::Type)i)).c_str());
        panels[0]->AppendChild(std::move(newPanel));
    }

    auto& buildCostDesc = GameplayRulesManager::Instance().FBuildingCostManager.GetBuildingCostDescriptors();
    forrange(i, 0, buildCostDesc.size())
    {
        auto& costRule = buildCostDesc[i];

        Rml::Element* tabToUse = FDocument->GetElementById(fmt::format("{}-panel", BuildingCategory::AsString(costRule.BuildingType())).c_str());
        AssertRelease(tabToUse != nullptr);
        Rml::ElementPtr newBuilding = FDocument->CreateElement("buildicon");
        newBuilding->SetId(costRule.BuildingTemplateName().c_str());
        newBuilding->SetInnerRML(costRule.BuildingTemplateName().c_str());
        tabToUse->AppendChild(std::move(newBuilding));
    }
}

} // namespace UI
} // namespace ECSEngine
