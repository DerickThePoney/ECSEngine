#include "stdafx.h"

#include "PhysicsBodyConfig.h"

namespace ECSEngine
{
namespace Physics
{

void PhysicsBodyConfig::DrawInEditor()
{
    ImGui::PushID(this);
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
    ImGui::PopID();
}

} // namespace Physics
} // namespace ECSEngine