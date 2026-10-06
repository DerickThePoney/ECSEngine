#include "stdafx.h"

#include "PhysicsBodyConfig.h"

namespace ECSEngine
{
namespace Physics
{

void PhysicsBodyConfig::DrawInEditor()
{
    ImGui::PushID(this);

    u32 lastIndex = EPhysicsMoveability::LENGTH;

    std::string currentMoveability = EPhysicsMoveability::AsString(FMoveability);
    if (ImGui::BeginCombo("##MOVEABILITY", currentMoveability.c_str()))
    {
        forrange(i, 0, lastIndex)
        {
            if (ImGui::Selectable(EPhysicsMoveability::AsString((EPhysicsMoveability::Type)i).c_str(), i == FMoveability))
            {
                FMoveability = (EPhysicsMoveability::Type)i;
            }
        }

        ImGui::EndCombo();
    }

    ImGui::Checkbox("Apply gravity", &FApplyGravity);
    ImGui::Checkbox("AutoCompute mass", &FAutoComputeMass);
    if (!FAutoComputeMass)
    {
        ImGui::SliderFloat("Mass", &FMass, 0.f, 500000.f);
    }
    else
    {
        ImGui::SliderFloat("Density", &FDensity, 0.f, 50.f);
    }

    auto ShapeDrawer = [](std::string& parName, CollisionShape& parShape)
    {
        ImGui::PushID(ImGui::GetID(&parName));
        if (ImGui::CollapsingHeader(parName.c_str()))
            parShape.DrawInEditor();
        ImGui::PopID();
    };

    EDITOR_PROPERTY_COMPLEXVECTOR(CollisionShape, "Shapes", FShapes, false, ShapeDrawer, true);

    ImGui::SliderFloat("Linear damping", &FLinearDamping, 0.f, 4.f);
    ImGui::SliderFloat("Angular damping", &FAngularDamping, 0.f, 4.f);

    ImGui::SliderFloat("Restitution", &FRestitution, 0.f, 1.f);
    ImGui::SliderFloat("Friction", &FFriction, 0.f, 1.f);
    ImGui::PopID();
}

} // namespace Physics
} // namespace ECSEngine