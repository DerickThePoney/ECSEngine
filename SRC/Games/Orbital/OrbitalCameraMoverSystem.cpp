#include "stdafx.h"

#include "OrbitalCameraMoverSystem.h"

#include "Common/CameraManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/ApparenceModule.h"
#include "ECSGameplay_Common/OrientationModule.h"
#include "ECSGameplay_Common/PositionModule.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "MapModeCameraMoverModule.h"
#include "Physics/PhysicsAPI.h"
#include "PlanetGravityModule.h"
#include "RenderingCore/Carrier.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/GFXRepresentation.h"
#include "RenderingCore/GFXRepresentationManager.h"
#include "RenderingCore/GFXRepresentationProxy.h"
#include "RocketCameraMoverModule.h"
#include "RocketManagerModule.h"

namespace ECSEngine
{
namespace
{
vec3 ResolveTangent(const vec3& up, const vec3& currentTangent, bool orbitInitialised, bool& outOrbitInitialised)
{
    vec3 tangent = currentTangent;
    if (!orbitInitialised)
    {
        const vec3 reference = (fabsf(Dot(up, vec3(0.f, 1.f, 0.f))) > 0.99f) ? vec3(1.f, 0.f, 0.f) : vec3(0.f, 1.f, 0.f);
        tangent = Normalize(Cross(reference, up));
        outOrbitInitialised = true;
        return tangent;
    }

    const vec3 projected = tangent - up * Dot(tangent, up);
    const float projectedLength = Length(projected);
    if (projectedLength > 1e-4f)
        return Normalize(projected);

    const vec3 fallback = (fabsf(up.x) < 0.9f) ? vec3(1.f, 0.f, 0.f) : vec3(0.f, 0.f, 1.f);
    return Normalize(Cross(fallback, up));
}

void ApplyOrbitInput(MouseButtonCommand& rotation,
      MouseScrollCommand& scroll,
      float& distance,
      float& pitch,
      float& yaw,
      float minPitch,
      float maxPitch,
      float minDistance,
      float maxDistance)
{
    if (rotation.Evaluate())
    {
        const vec2 delta = Input::GetMousePositionDelta();
        constexpr float sensitivity = 0.005f;
        yaw -= delta.x * sensitivity;
        pitch -= delta.y * sensitivity;
    }
    pitch = Clamp(pitch, minPitch, maxPitch);
    if (scroll.Evaluate())
    {
        const float scrollDelta = Input::GetMouseScrollDelta().y;
        distance *= expf(-scrollDelta * 0.12f);
    }
    distance = Clamp(distance, minDistance, maxDistance);
}

void WriteOrbitCamera(PositionModule& position,
      OrientationModule& orientation,
      u32 cameraId,
      const vec3& pivot,
      const vec3& up,
      const vec3& tangent,
      float distance,
      float pitch,
      float yaw)
{
    const vec3 bitangent = Normalize(Cross(up, tangent));
    const vec3 horizontal = (tangent * Cos(yaw)) + (bitangent * Sin(yaw));
    const vec3 offset = (horizontal * (Cos(pitch) * distance)) + (up * (Sin(pitch) * distance));
    const vec3 eye = pivot + offset;

    const mat4 lookAtMatrix = LookAt(eye, pivot, up);
    const mat4 lookAtRotation = GetRotationAsMat4(lookAtMatrix);
    position.SetPosition3D(eye);
    orientation.SetOrientation(quat::FromMat4(Invert(lookAtRotation)));

    Camera* camera = CameraManager::Instance().GetCamera(cameraId);
    camera->SetWorldViewMatrix(lookAtMatrix);
}

bool CarrierPosition(ApparenceModule* appearance, vec3& outPosition)
{
    if (appearance == nullptr || appearance->Proxy() == nullptr)
        return false;

    Rendering::GFXRepresentation* gfx = Rendering::GFXRepresentationManager::Instance().GetGFX(appearance->Proxy()->Id());
    const Rendering::Carrier* carrier = (gfx != nullptr) ? gfx->GetCarrier() : nullptr;
    if (carrier == nullptr)
        return false;

    outPosition = carrier->Position();
    return true;
}
} // namespace

OrbitalCameraMoverSystem::OrbitalCameraMoverSystem()
{
    RegisterDependency<RocketCameraMoverModule>(EEntityWorlds::CAMERA);
    RegisterDependency<MapModeCameraMoverModule>(EEntityWorlds::CAMERA);
    RegisterDependency<PositionModule>(EEntityWorlds::CAMERA);
    RegisterDependency<OrientationModule>(EEntityWorlds::CAMERA);

    RegisterDependency<RocketManagerModule>(EEntityWorlds::STANDARD);
    RegisterDependency<ApparenceModule>(EEntityWorlds::STANDARD);
    RegisterDependency<PlanetGravityModule>(EEntityWorlds::STANDARD);
    RegisterDependency<RigidbodyModule>(EEntityWorlds::STANDARD);

    FMiddleMouseRotation.FMouseButton = MouseButtons::MOUSE_BUTTON_3;
    FMiddleMouseRotation.FInputType = EInputType::REPEATED;

    FMapModeChange.FKeyboardKey = InputKeyNames::INPUT_KEY_M;
    FMapModeChange.FInputType = EInputType::PRESSED;
}

void OrbitalCameraMoverSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    if (FMapModeChange.Evaluate())
        FCameraMode = (ECameraMode)(((u8)FCameraMode + 1) % (u8)ECameraMode::LENGTH);

    switch (FCameraMode)
    {
    case ECameraMode::ROCKET_CHASE:
        UpdateRocketChase();
        break;
    case ECameraMode::MAP_MODE:
        UpdateMapMode();
        break;
    default:
        AssertRelease(false);
        break;
    }
}

void OrbitalCameraMoverSystem::UpdateRocketChase()
{
    ModuleAccessor<RocketCameraMoverModule> rocketCameraMoverAccessor(EEntityWorlds::CAMERA);
    ModuleAccessor<PositionModule> positionAccessor(EEntityWorlds::CAMERA);
    ModuleAccessor<OrientationModule> orientationAccessor(EEntityWorlds::CAMERA);
    AssertRelease(rocketCameraMoverAccessor.size() <= 1);

    ModuleAccessor<RocketManagerModule> rocketManagerAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<ApparenceModule> appearanceAccessor(EEntityWorlds::STANDARD);
    AssertRelease(rocketManagerAccessor.size() <= 1);

    if (rocketManagerAccessor.size() == 0)
        return;

    foreachitem(moverModule, rocketCameraMoverAccessor)
    {
        const RocketManagerModule* rocket = rocketManagerAccessor[moverModule.TargetedRocket()];
        AssertRelease(rocket != nullptr);

        vec3 pivot;
        if (!CarrierPosition(appearanceAccessor[rocket->RootId()], pivot))
            return;

        // Should use closest planet
        const vec3 up = Normalize(Invert(rocket->LastTurnGravity()));

        bool orbitInitialised = moverModule.OrbitInitialised();
        const vec3 tangent = ResolveTangent(up, moverModule.Tangent(), moverModule.OrbitInitialised(), orbitInitialised);
        moverModule.SetOrbitInitialised(orbitInitialised);
        moverModule.SetTangent(tangent);

        float distance = moverModule.Distance();
        float pitch = moverModule.Pitch();
        float yaw = moverModule.Yaw();
        ApplyOrbitInput(FMiddleMouseRotation, FMiddleMouseScroll, distance, pitch, yaw, Radians(-80.f), Radians(80.f), 5.f, 8000.f);
        moverModule.SetDistance(distance);
        moverModule.SetPitch(pitch);
        moverModule.SetYaw(yaw);

        OrientationModule* cameraOrientation = orientationAccessor[moverModule.UnitId()];
        AssertRelease(cameraOrientation != nullptr);
        PositionModule* cameraPosition = positionAccessor[moverModule.UnitId()];
        AssertRelease(cameraPosition != nullptr);

        WriteOrbitCamera(*cameraPosition, *cameraOrientation, moverModule.CamId(), pivot, up, tangent, distance, pitch, yaw);

        if (FMapNearActive)
        {
            Camera* camera = CameraManager::Instance().GetCamera(moverModule.CamId());
            camera->SetNear(FStoredNear);
            FMapNearActive = false;
        }
    }
}

void OrbitalCameraMoverSystem::UpdateMapMode()
{
    ModuleAccessor<MapModeCameraMoverModule> mapAccessor(EEntityWorlds::CAMERA);
    ModuleAccessor<PositionModule> positionAccessor(EEntityWorlds::CAMERA);
    ModuleAccessor<OrientationModule> orientationAccessor(EEntityWorlds::CAMERA);
    AssertRelease(mapAccessor.size() <= 1);
    if (mapAccessor.size() == 0)
        return;

    ModuleAccessor<PlanetGravityModule> planetAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<ApparenceModule> appearanceAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<RigidbodyModule> rigidbodyAccessor(EEntityWorlds::STANDARD);

    foreachitem(mapModule, mapAccessor)
    {
        if (!mapModule.TargetedPlanet().Valid())
        {
            if (planetAccessor.size() == 0)
                return;
            mapModule.SetTargetedPlanet(planetAccessor.begin().GetRef().UnitId());
        }

        const EntityId planetId = mapModule.TargetedPlanet();
        vec3 pivot;
        if (!CarrierPosition(appearanceAccessor[planetId], pivot))
        {
            const RigidbodyModule* planetBody = rigidbodyAccessor[planetId];
            AssertRelease(planetBody != nullptr);
            Physics::GetBodyCenterOfMass(planetBody->BodyHandle(), pivot);
        }

        const vec3 up(0.f, 1.f, 0.f);

        bool orbitInitialised = mapModule.OrbitInitialised();
        const vec3 tangent = ResolveTangent(up, mapModule.Tangent(), mapModule.OrbitInitialised(), orbitInitialised);
        mapModule.SetOrbitInitialised(orbitInitialised);
        mapModule.SetTangent(tangent);

        float distance = mapModule.Distance();
        float pitch = mapModule.Pitch();
        float yaw = mapModule.Yaw();
        ApplyOrbitInput(FMiddleMouseRotation, FMiddleMouseScroll, distance, pitch, yaw, Radians(-89.f), Radians(89.f), 50.f, 50000.f);
        mapModule.SetDistance(distance);
        mapModule.SetPitch(pitch);
        mapModule.SetYaw(yaw);

        OrientationModule* cameraOrientation = orientationAccessor[mapModule.UnitId()];
        AssertRelease(cameraOrientation != nullptr);
        PositionModule* cameraPosition = positionAccessor[mapModule.UnitId()];
        AssertRelease(cameraPosition != nullptr);

        WriteOrbitCamera(*cameraPosition, *cameraOrientation, mapModule.CamId(), pivot, up, tangent, distance, pitch, yaw);

        Camera* camera = CameraManager::Instance().GetCamera(mapModule.CamId());
        if (!FMapNearActive)
        {
            FStoredNear = camera->Near();
            FMapNearActive = true;
        }
        camera->SetNear(Max(0.1f, distance * 1.e-3f));
    }
}

void OrbitalCameraMoverSystem::DrawDebug(Rendering::DrawCommandBuffer* buffer, const Rendering::MaterialInstanceHandle& material)
{
    if (FCameraMode != ECameraMode::MAP_MODE)
        return;

    LockControllers();

    ModuleAccessor<RocketManagerModule> rocketManagerAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<RigidbodyModule> rigidbodyAccessor(EEntityWorlds::STANDARD);

    foreachitem(rocket, rocketManagerAccessor)
    {
        vec3 rocketPosition;
        if (!rocket.ComputeActiveCenterOfMass(rigidbodyAccessor, rocketPosition))
            continue;
        buffer->DrawDebugSphere(material, rocketPosition, 200.f, 0xFFFFFFFFu);
    }

    UnlockControllers();
}

} // namespace ECSEngine
