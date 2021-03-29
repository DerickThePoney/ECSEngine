#include "stdafx.h"

#include "BuildMenuController.h"

#include "BuildingCostManager.h"
#include "GameplayRulesManager.h"
#include "MousePolicyManager.h"
#include "PlaceBuildingMousePolicy.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"

namespace ECSEngine
{
namespace UI
{

BuildMenuController::BuildMenuController()
    : FSize(0.6f, 0.2f)
    , FPosition(0.5f, 0.9f)
{
    FPosition.WindowAnchor = glm::vec2(0.5f, 1.f);
}

void BuildMenuController::VirtualUpdate()
{
    UIController::VirtualUpdate();

    glm::uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    FPosition.PushPos(windowSize);
    FSize.PushSize(windowSize);

    ImGui::Begin("Build Menu");

    foreachitemconst(costRule, GameplayRulesManager::Instance().FBuildingCostManager.GetBuildingCostDescriptors())
    {
        if (ImGui::Button(costRule.BuildingTemplateName().c_str()))
        {
            PlaceBuildingMousePolicy* mousePolicy = MousePolicyManager::Instance().GetMousePolicy<PlaceBuildingMousePolicy>(MousePolicyType::PLACE_BUILDING);
            if (mousePolicy->IsActivated())
                mousePolicy->Deactivate();
            mousePolicy->SetupMousePolicy(costRule.BuildingTemplateName());
            mousePolicy->Activate();
        }
    }

    ImGui::End();
}

} // namespace UI
} // namespace ECSEngine
