#include "stdafx.h"

#include "PhysicsBodyConfig.h"

namespace ECSEngine
{
namespace Physics
{
vec3 PhysicsBodyConfig::ComputeCoMLocal() const
{
    return FShape.GetCenter();
}

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

    FShape.DrawInEditor();

    ImGui::SliderFloat("Linear damping", &FLinearDamping, 0.f, 4.f);
    ImGui::SliderFloat("Angular damping", &FAngularDamping, 0.f, 4.f);

    ImGui::SliderFloat("Restitution", &FRestitution, 0.f, 1.f);
    ImGui::SliderFloat("Friction", &FFriction, 0.f, 1.f);
    ImGui::PopID();
}

} // namespace Physics
} // namespace ECSEngine