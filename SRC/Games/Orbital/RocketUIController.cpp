#include "stdafx.h"

#include "RocketUIController.h"

#include "ECSCore/ScopedModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "Physics/PhysicsAPI.h"
#include "PlanetGravityModule.h"
#include "RenderingCore/DrawCommands.h"
#include "RocketManagerModule.h"

namespace ECSEngine
{
namespace UI
{
using AccessHelpers =
      ScopedModuleAccessor<MC<RocketManagerModule, EEntityWorlds::STANDARD>, MC<RigidbodyModule, EEntityWorlds::STANDARD>, MC<PlanetGravityModule, EEntityWorlds::STANDARD>>;

void RocketUIController::VirtualUpdate()
{
    UIController::VirtualUpdate();

    AccessHelpers accessHelpers;

    auto& PlanetAccessor = accessHelpers.Accessor<PlanetGravityModule>();
    if (PlanetAccessor.size() == 0 || PlanetAccessor.size() > 1)
        return;

    PlanetGravityModule& planetModule = *PlanetAccessor.begin();
    const RigidbodyModule* planetRigidbodyModule = accessHelpers.GetModule<RigidbodyModule>(planetModule.UnitId());
    vec3 PlanetPosition;
    Physics::GetBodyCenterOfMass(planetRigidbodyModule->BodyHandle(), PlanetPosition);

    ImGui::Begin("Rocket UI");

    u32 rocketIndex = 0;
    foreachitem(rocketManager, accessHelpers.Accessor<RocketManagerModule>())
    {
        ImGui::SeparatorText(std::format("Rocket {}", rocketIndex++).c_str());
        ImGui::Indent();

        vec3 RocketPosition;
        vec3 RocketVelocity;
        const bool hasVesselCenterOfMass = rocketManager.ComputeCenterOfMass(accessHelpers.Accessor<RigidbodyModule>(), RocketPosition, &RocketVelocity);
        AssertRelease(hasVesselCenterOfMass);
        if (!hasVesselCenterOfMass)
        {
            ImGui::Unindent();
            continue;
        }

        const vec3 PlanetToRocket = RocketPosition - PlanetPosition;
        const float LPlanetToRocket = Length(PlanetToRocket);
        const vec3 UpDirection = Normalize(PlanetToRocket);
        const float Altitude = LPlanetToRocket - planetModule.PlanetRadius();
        const float VerticalSpeed = Dot(RocketVelocity, UpDirection);
        const float HorizontalSpeed = Length(RocketVelocity - VerticalSpeed * UpDirection);

        ImGui::SeparatorText("Altitude and speed data");
        ImGui::Text("Altitude : %.3f m", Altitude);
        ImGui::Text("Vertical speed : %.3f m/s", VerticalSpeed);
        ImGui::Text("Horizontal speed : %.3f m/s", HorizontalSpeed);

        const float rocketSpeedSq = LengthSq(RocketVelocity);
        const float energy = 0.5f * rocketSpeedSq - planetModule.PlanetGravityConstant() / LPlanetToRocket; // ε
        const vec3 h = Cross(PlanetToRocket, RocketVelocity); // specific angular momentum
        const float hSq = Dot(h, h);
        const float eccentricity = sqrtf(1.f + 2.f * energy * hSq / (planetModule.PlanetGravityConstant() * planetModule.PlanetGravityConstant()));
        const float semiMajor = -planetModule.PlanetGravityConstant() / (2.f * energy); // a, negative on a hyperbola
        const float periapsisRadius = semiMajor * (1.f - eccentricity);
        const float periapsisAltitude = periapsisRadius - planetModule.PlanetRadius();

        const float apoapsisAltitude = semiMajor * (1.f + eccentricity) - planetModule.PlanetRadius();

        ImGui::SeparatorText("Trajectory data");
        ImGui::Text("Energy : %.3f", energy);
        ImGui::Text("Eccentricity : %.3f", eccentricity);
        ImGui::Text("Periapsis : %.3f", periapsisAltitude);
        ImGui::Text("Apoapsis : %.3f", (energy < 0) ? apoapsisAltitude : -1.f);

        FHasPeriapsis = false;
        FHasApoapsis = false;
        FOrbitPoints.clear();

        ImGui::Checkbox("Draw predicted trajectory", &FDrawPredictedPath);

        ImGui::SeparatorText("Engine data");
        ImGui::Text("Engine activated : %d", rocketManager.EngineFiring());

        ImGui::SeparatorText("SAS data");
        bool bSASActivated = rocketManager.SASActivated();
        ImGui::Checkbox("SAS Activated", &bSASActivated);
        rocketManager.SetSASActivated(bSASActivated);

        int SASMode = (int)rocketManager.SASMode();
        ImGui::RadioButton("Stability", &SASMode, (int)ESASMode::STABILITY);
        ImGui::SameLine();
        ImGui::RadioButton("Prograde", &SASMode, (int)ESASMode::PROGRADE);
        ImGui::SameLine();
        ImGui::RadioButton("Retrograde", &SASMode, (int)ESASMode::RETROGRADE);
        ImGui::RadioButton("Radial out", &SASMode, (int)ESASMode::RADIALOUT);
        ImGui::SameLine();
        ImGui::RadioButton("Radial in", &SASMode, (int)ESASMode::RADIALIN);
        ImGui::SameLine();
        ImGui::RadioButton("Normal", &SASMode, (int)ESASMode::NORMAL);
        ImGui::SameLine();
        ImGui::RadioButton("Anti-normal", &SASMode, (int)ESASMode::ANTINORMAL);

        rocketManager.SetSASMode((ESASMode)SASMode);

        ImGui::Checkbox("Draw SAS Target", &FDrawSASTarget);
        FHasSASTargetDraw = false;
        if (bSASActivated && FDrawSASTarget && rocketManager.HasSASTarget())
        {
            const vec3 nose = (mat4(rocketManager.SASTarget()) * vec4(0.f, 1.f, 0.f, 0.f)).xyz();
            FSASTargetOrigin = RocketPosition;
            FSASTargetDirection = Normalize(nose);
            FHasSASTargetDraw = true;
        }

        if (FDrawPredictedPath)
        {
            const float mu = planetModule.PlanetGravityConstant();
            const vec3 eVec = Cross(RocketVelocity, h) * (1.f / mu) - UpDirection;
            vec3 periDir;
            if (LengthSq(eVec) < 1.e-8f)
            {
                const vec3 fallback = (fabsf(h.y) < 0.9f * Length(h)) ? vec3(0.f, 1.f, 0.f) : vec3(1.f, 0.f, 0.f);
                periDir = Normalize(Cross(h, fallback));
            }
            else
            {
                periDir = Normalize(eVec);
            }
            FPeriapsis = PlanetPosition + periDir * periapsisRadius;
            FHasPeriapsis = true;
            if (energy < 0.f && hSq > 1.e-4f)
            {
                const vec3 motionDir = Normalize(Cross(h, periDir));
                const float eDraw = Min(eccentricity, 0.999f);
                const float p = hSq / mu;
                constexpr u32 sampleCount = 128;
                FOrbitPoints.resize(sampleCount);
                for (u32 i = 0; i < sampleCount; ++i)
                {
                    const float theta = (float)i / (float)sampleCount * 2.f * Pi();
                    const float cosTheta = Cos(theta);
                    const float sinTheta = Sin(theta);
                    const float rho = p / Max(1.f + eDraw * cosTheta, 1.e-4f);
                    const vec3 direction = periDir * cosTheta + motionDir * sinTheta;
                    FOrbitPoints[i] = PlanetPosition + direction * rho;
                }
                FApoapsis = PlanetPosition - periDir * (semiMajor * (1.f + eccentricity));
                FHasApoapsis = true;
            }
        }
        ImGui::Unindent();
    }

    ImGui::End();
}

void RocketUIController::DrawDebug(Rendering::DrawCommandBuffer* buffer, const Rendering::MaterialInstanceHandle& material) const
{
    if (!FOrbitPoints.empty())
        buffer->DrawLines(material, FOrbitPoints.data(), (u32)FOrbitPoints.size(), 0xFFFFFF00u, true);
    if (FHasPeriapsis)
        buffer->DrawDebugSphere(material, FPeriapsis, 10.f, 0xFF00FF00u);
    if (FHasApoapsis)
        buffer->DrawDebugSphere(material, FApoapsis, 10.f, 0xFF0000FFu);

    if (FHasSASTargetDraw)
        buffer->DrawDebugArrow(material, FSASTargetOrigin, FSASTargetDirection, 20.f, 0xFF00FFFFu);
}
} // namespace UI
} // namespace ECSEngine
