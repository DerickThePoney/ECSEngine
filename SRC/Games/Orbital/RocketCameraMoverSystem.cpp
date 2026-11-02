#include "stdafx.h"

#include "RocketCameraMoverSystem.h"

#include "Common/CameraManager.h"
#include "Common/TimeManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/ApparenceModule.h"
#include "ECSGameplay_Common/CameraMoverModule.h"
#include "ECSGameplay_Common/OrientationModule.h"
#include "ECSGameplay_Common/PositionModule.h"
#include "RenderingCore/Carrier.h"
#include "RenderingCore/GFXRepresentation.h"
#include "RenderingCore/GFXRepresentationManager.h"
#include "RenderingCore/GFXRepresentationProxy.h"
#include "RocketManagerModule.h"

namespace ECSEngine
{

RocketCameraMoverSystem::RocketCameraMoverSystem()
{
    RegisterDependency<CameraMoverModule>(EEntityWorlds::CAMERA);
    RegisterDependency<PositionModule>(EEntityWorlds::CAMERA);
    RegisterDependency<OrientationModule>(EEntityWorlds::CAMERA);

    RegisterDependency<RocketManagerModule>(EEntityWorlds::STANDARD);
    RegisterDependency<ApparenceModule>(EEntityWorlds::STANDARD);
    RegisterDependency<PositionModule>(EEntityWorlds::STANDARD);
    RegisterDependency<OrientationModule>(EEntityWorlds::STANDARD);

    FMiddleMouseRotation.FMouseButton = MouseButtons::MOUSE_BUTTON_3;
    FMiddleMouseRotation.FInputType = EInputType::REPEATED;
}

void RocketCameraMoverSystem::VirtualInit()
{
    ModuleSystem::VirtualInit();
    FOrbitInitialised = false;
}

void RocketCameraMoverSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<CameraMoverModule> cameraMoverAccessor(EEntityWorlds::CAMERA);
    ModuleAccessor<PositionModule> positionAccessor(EEntityWorlds::CAMERA);
    ModuleAccessor<OrientationModule> orientationAccessor(EEntityWorlds::CAMERA);
    AssertRelease(cameraMoverAccessor.size() <= 1); // Pour l'instant on en prend qu'un !!

    ModuleAccessor<RocketManagerModule> rocketManagerAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<ApparenceModule> appearanceAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<PositionModule> rocketPositionAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<OrientationModule> rocketOrientationAccessor(EEntityWorlds::STANDARD);
    AssertRelease(rocketManagerAccessor.size() <= 1); // Pour l'instant on en prend qu'un !!

    // Compute the pivot and UP vector
    if (rocketManagerAccessor.size() == 0)
        return;
    const RocketManagerModule& rocket = *rocketManagerAccessor.begin();

    ApparenceModule* appearance = appearanceAccessor[rocket.BodyId()];
    if (appearance == nullptr || appearance->Proxy() == nullptr)
        return;

    Rendering::GFXRepresentation* gfx = Rendering::GFXRepresentationManager::Instance().GetGFX(appearance->Proxy()->Id());
    const Rendering::Carrier* carrier = (gfx != nullptr) ? gfx->GetCarrier() : nullptr;
    if (carrier == nullptr)
        return;
    const vec3 pivot = carrier->Position();

    vec3 UpVector = vec3(0.f, 1.f, 0.f);

    // Stable axes of the orbit plane. Yaw is an angle in this plane.
    const vec3 reference = (fabsf(Dot(UpVector, vec3(0.f, 1.f, 0.f))) > 0.99f) ? vec3(1.f, 0.f, 0.f) : vec3(0.f, 1.f, 0.f);
    const vec3 tangent = Normalize(Cross(UpVector, reference));
    const vec3 bitangent = Normalize(Cross(UpVector, tangent));

    const float deltaTime = TimeManager::FrameDeltaTime();
    foreachitem(moverModule, cameraMoverAccessor)
    {
        OrientationModule* cameraOrientationModule = orientationAccessor[moverModule.UnitId()];
        AssertRelease(cameraOrientationModule != nullptr);
        PositionModule* cameraPositionModule = positionAccessor[moverModule.UnitId()];
        AssertRelease(cameraPositionModule != nullptr);

        if (!FOrbitInitialised)
        {
            const vec3 offset = cameraPositionModule->GetPosition3D() - pivot;
            const float distance = Length(offset);

            if (distance > 0.01f)
            {
                const vec3 direction = offset * 1.f / distance;
                FDistance = distance;
                FPitch = asinf(Clamp(Dot(direction, UpVector), -1.f, 1.f));
                const vec3 horizontal = direction - UpVector * Sin(FPitch);
                if (Length(horizontal) > 1e-4f)
                {
                    const vec3 flat = Normalize(horizontal);
                    FYaw = atan2f(Dot(flat, bitangent), Dot(flat, tangent));
                }
            }
            else
            {
                FDistance = 30.f;
                FPitch = Radians(25.f);
                FYaw = 0.f;
            }
            FOrbitInitialised = true;
        }

        // Pixels, not a normalized direction, and not scaled by delta time.
        if (FMiddleMouseRotation.Evaluate())
        {
            const vec2 delta = Input::GetMousePositionDelta();
            constexpr float sensitivity = 0.005f;
            FYaw -= delta.x * sensitivity;
            FPitch -= delta.y * sensitivity;
        }
        FPitch = Clamp(FPitch, Radians(-80.f), Radians(80.f));
        if (FMiddleMouseScroll.Evaluate())
        {
            const float scroll = Input::GetMouseScrollDelta().y;
            FDistance *= expf(-scroll * 0.12f);
        }
        FDistance = Clamp(FDistance, 5.f, 8000.f);

        const vec3 horizontal = (tangent * Cos(FYaw)) + (bitangent * Sin(FYaw));
        const vec3 offset = (horizontal * (Cos(FPitch) * FDistance)) + (UpVector * (Sin(FPitch) * FDistance));
        const vec3 eye = pivot + offset;

        const vec3 forward = Normalize(pivot - eye);
        const vec3 rightRaw = Cross(UpVector, forward);
        if (Dot(rightRaw, rightRaw) < 1.e-6f)
            return;

        // Same column meaning as CameraMoverSystem: 0 right, 1 up, 2 look direction.
        const vec3 right = Normalize(rightRaw);
        const vec3 cameraUp = Cross(forward, right);
        mat4 basis = mat4::Identity();
        basis.SetColumn(0, vec4(right, 0.f));
        basis.SetColumn(1, vec4(cameraUp, 0.f));
        basis.SetColumn(2, vec4(forward, 0.f));
        cameraPositionModule->SetPosition3D(eye);
        cameraOrientationModule->SetOrientation(quat::FromMat4(basis));

        mat4 world = basis;
        world.SetColumn(3, vec4(eye, 1.f));
        Camera* camera = CameraManager::Instance().GetCamera(moverModule.CamId());
        camera->SetWorldViewMatrix(Invert(world));
    }
}

} // namespace ECSEngine