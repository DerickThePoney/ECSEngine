#include "stdafx.h"

#include "RocketRecipe.h"

#include "ECSCore/ECSCorePropertyDrawer.h"
#include "ECSCore/EntityTemplateManager.h"

namespace ECSEngine
{
void RocketPartRecipe::FetchEntityTemplate()
{
    FEntityTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FEntityTemplateName);
}

void RocketPartRecipe::DrawInEditor()
{
    EDITOR_PROPERTY_ENTITY_TEMPLATE("Rocket part template", FEntityTemplate, FEntityTemplateName);

    ImGui::SeparatorText("Part Role");
    int value = (int)FRole;
    ImGui::RadioButton("NO ROLE", &value, 0);
    ImGui::SameLine();
    ImGui::RadioButton("FUEL TANK", &value, 1);
    ImGui::SameLine();
    ImGui::RadioButton("ENGINE", &value, 2);
    FRole = (ERocketPartRole)value;

    if (FRole == ERocketPartRole::ENGINE)
    {
        EDITOR_PROPERTY_SIMPLE("Fuel tank part index", FFuelTankIndex);
    }

    ImGui::SeparatorText("Joint type");
    value = (int)FJointType;
    ImGui::RadioButton("NO JOINT", &value, 0);
    ImGui::SameLine();
    ImGui::RadioButton("BALL JOINT", &value, 1);
    ImGui::SameLine();
    ImGui::RadioButton("WELD JOINT", &value, 2);
    FJointType = (ERocketPartJoint)value;

    if (FJointType != ERocketPartJoint::NONE)
    {
        if (ImGui::CollapsingHeader("Joint config"))
        {
            switch (FJointType)
            {
            case ERocketPartJoint::BALL:
                FBallConfig.DrawInEditor();
                break;
            case ERocketPartJoint::WELD:
                FWeldConfig.DrawInEditor();
                break;
            }
        }

        ImGui::SeparatorText("Decoupling");
        EDITOR_PROPERTY_BOOL("Is decoupler?", FbDecoupler);
        if (FbDecoupler)
        {
            EDITOR_PROPERTY_SIMPLE("Separation speed", FSeparationSpeed);
            if (FSeparationSpeed < 0.1f)
                FSeparationSpeed = 0.1f;
        }
    }
}

} // namespace ECSEngine
