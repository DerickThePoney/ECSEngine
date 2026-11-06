#include "stdafx.h"

#include "RocketCameraMoverSystem.h"

#include "Common/CameraManager.h"
#include "Common/TimeManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/ApparenceModule.h"
#include "ECSGameplay_Common/OrientationModule.h"
#include "ECSGameplay_Common/PositionModule.h"
#include "RenderingCore/Carrier.h"
#include "RenderingCore/GFXRepresentation.h"
#include "RenderingCore/GFXRepresentationManager.h"
#include "RenderingCore/GFXRepresentationProxy.h"
#include "RocketCameraMoverModule.h"
#include "RocketManagerModule.h"

namespace ECSEngine
{

RocketCameraMoverSystem::RocketCameraMoverSystem()
{
    RegisterDependency<RocketCameraMoverModule>(EEntityWorlds::CAMERA);
    RegisterDependency<PositionModule>(EEntityWorlds::CAMERA);
    RegisterDependency<OrientationModule>(EEntityWorlds::CAMERA);

    RegisterDependency<RocketManagerModule>(EEntityWorlds::STANDARD);
    RegisterDependency<ApparenceModule>(EEntityWorlds::STANDARD);

    FMiddleMouseRotation.FMouseButton = MouseButtons::MOUSE_BUTTON_3;
    FMiddleMouseRotation.FInputType = EInputType::REPEATED;
}

void RocketCameraMoverSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<RocketCameraMoverModule> rocketCameraMoverAccessor(EEntityWorlds::CAMERA);
    ModuleAccessor<PositionModule> positionAccessor(EEntityWorlds::CAMERA);
    ModuleAccessor<OrientationModule> orientationAccessor(EEntityWorlds::CAMERA);
    AssertRelease(rocketCameraMoverAccessor.size() <= 1); // Pour l'instant on en prend qu'un !!

    ModuleAccessor<RocketManagerModule> rocketManagerAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<ApparenceModule> appearanceAccessor(EEntityWorlds::STANDARD);
    AssertRelease(rocketManagerAccessor.size() <= 1); // Pour l'instant on en prend qu'un !!

    // Compute the pivot and UP vector
    if (rocketManagerAccessor.size() == 0)
        return;

    const float deltaTime = TimeManager::FrameDeltaTime();
    foreachitem(moverModule, rocketCameraMoverAccessor)
    {
        const RocketManagerModule* rocket = rocketManagerAccessor[moverModule.TargetedRocket()];
        AssertRelease(rocket != nullptr);

        ApparenceModule* appearance = appearanceAccessor[rocket->BodyId()];
        if (appearance == nullptr || appearance->Proxy() == nullptr)
            return;

        Rendering::GFXRepresentation* gfx = Rendering::GFXRepresentationManager::Instance().GetGFX(appearance->Proxy()->Id());
        const Rendering::Carrier* carrier = (gfx != nullptr) ? gfx->GetCarrier() : nullptr;
        if (carrier == nullptr)
            return;
        const vec3 pivot = carrier->Position();

        // Should use closest planet
        vec3 UpVector = Normalize(Invert(rocket->LastTurnGravity()));

        // Yaw is an angle in the plane perpendicular to radial up.
        // The first frame locks that angle to the placed camera. After that the yaw axis is
        // projected onto the new plane. Rebuilding it from a fixed world axis flips 180 degrees
        // once radial up leaves world +Y, which mirrors the view.
        vec3 Tangent = moverModule.Tangent();
        if (!moverModule.OrbitInitialised())
        {
            const vec3 reference = (fabsf(Dot(UpVector, vec3(0.f, 1.f, 0.f))) > 0.99f) ? vec3(1.f, 0.f, 0.f) : vec3(0.f, 1.f, 0.f);
            Tangent = Normalize(Cross(reference, UpVector));
            moverModule.SetOrbitInitialised(true);
        }
        else
        {
            const vec3 projected = Tangent - UpVector * Dot(Tangent, UpVector);
            const float projectedLength = Length(projected);
            if (projectedLength > 1e-4f)
                Tangent = Normalize(projected);
            else
            {
                const vec3 fallback = (fabsf(UpVector.x) < 0.9f) ? vec3(1.f, 0.f, 0.f) : vec3(0.f, 0.f, 1.f);
                Tangent = Normalize(Cross(fallback, UpVector));
            }
        }
        const vec3 bitangent = Normalize(Cross(UpVector, Tangent));
        moverModule.SetTangent(Tangent);

        OrientationModule* cameraOrientationModule = orientationAccessor[moverModule.UnitId()];
        AssertRelease(cameraOrientationModule != nullptr);
        PositionModule* cameraPositionModule = positionAccessor[moverModule.UnitId()];
        AssertRelease(cameraPositionModule != nullptr);

        float Distance = moverModule.Distance(), Pitch = moverModule.Pitch(), Yaw = moverModule.Yaw();
        // Pixels, not a normalized direction, and not scaled by delta time.
        if (FMiddleMouseRotation.Evaluate())
        {
            const vec2 delta = Input::GetMousePositionDelta();
            constexpr float sensitivity = 0.005f;
            Yaw -= delta.x * sensitivity;
            Pitch -= delta.y * sensitivity;
        }
        Pitch = Clamp(Pitch, Radians(-80.f), Radians(80.f));
        if (FMiddleMouseScroll.Evaluate())
        {
            const float scroll = Input::GetMouseScrollDelta().y;
            Distance *= expf(-scroll * 0.12f);
        }
        Distance = Clamp(Distance, 5.f, 8000.f);

        moverModule.SetDistance(Distance);
        moverModule.SetPitch(Pitch);
        moverModule.SetYaw(Yaw);

        const vec3 horizontal = (Tangent * Cos(Yaw)) + (bitangent * Sin(Yaw));
        const vec3 offset = (horizontal * (Cos(Pitch) * Distance)) + (UpVector * (Sin(Pitch) * Distance));
        const vec3 eye = pivot + offset;

        mat4 LookAtMatrix = LookAt(eye, pivot, UpVector);

        mat4 LookAtRotation = GetRotationAsMat4(LookAtMatrix);
        cameraPositionModule->SetPosition3D(eye);
        cameraOrientationModule->SetOrientation(quat::FromMat4(Invert(LookAtRotation)));

        Camera* camera = CameraManager::Instance().GetCamera(moverModule.CamId());
        camera->SetWorldViewMatrix(LookAtMatrix);
    }
}

} // namespace ECSEngine