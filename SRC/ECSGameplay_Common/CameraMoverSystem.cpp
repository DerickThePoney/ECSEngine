#include "stdafx.h"

#include "CameraMoverSystem.h"

#include "CameraMoverModule.h"
#include "Common/CameraManager.h"
#include "Common/Logger.h"
#include "Common/TimeManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "OrientationModule.h"
#include "PositionModule.h"

namespace ECSEngine
{

CameraMoverSystem::CameraMoverSystem()
{
    RegisterDepency<CameraMoverModule>(EEntityWorlds::CAMERA);
    RegisterDepency<PositionModule>(EEntityWorlds::CAMERA);
    RegisterDepency<OrientationModule>(EEntityWorlds::CAMERA);

    FRight.FKeyboardKey = InputKeyNames::INPUT_KEY_A;
    FRight.FInputType = EInputType::REPEATED;

    FLeft.FKeyboardKey = InputKeyNames::INPUT_KEY_D;
    FLeft.FInputType = EInputType::REPEATED;

    FUp.FKeyboardKey = InputKeyNames::INPUT_KEY_W;
    FUp.FInputType = EInputType::REPEATED;

    FDown.FKeyboardKey = InputKeyNames::INPUT_KEY_S;
    FDown.FInputType = EInputType::REPEATED;

    FMiddleMouseRotation.FMouseButton = MouseButtons::MOUSE_BUTTON_3;
    FMiddleMouseRotation.FInputType = EInputType::REPEATED;
}

void CameraMoverSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<CameraMoverModule> cameraMoverAccessor(EEntityWorlds::CAMERA);
    ModuleAccessor<PositionModule> positionAccessor(EEntityWorlds::CAMERA);
    ModuleAccessor<OrientationModule> orientationAccessor(EEntityWorlds::CAMERA);
    AssertRelease(cameraMoverAccessor.size() <= 1); // Pour l'instant on en prend qu'un !!

    const float deltaTime = TimeManager::FrameDeltaTime();
    foreachitem(moverModule, cameraMoverAccessor)
    {
        OrientationModule* orientationModule = orientationAccessor[moverModule.UnitId()];
        AssertRelease(orientationModule != nullptr);
        PositionModule* positionModule = positionAccessor[moverModule.UnitId()];
        AssertRelease(positionModule != nullptr);

        const CameraMoverModuleTemplate* moverTemplate = moverModule.Template<CameraMoverModuleTemplate>();

        const bool slowMo = Input::IsCtrlDown();
        const float speedFactor = (slowMo) ? 0.2f : 1.0f;

        bool overrideWorldViewMatrix = false;
        vec3 movementCommand = vec3(0.0f);
        vec2 rotationScreenDirection = vec2(0.0f);

        rotationScreenDirection = Input::GetMousePositionDelta();
        mat4 newViewWorldMatrix = mat4::Identity();

        if (FMiddleMouseRotation.Evaluate())
        {
            if (rotationScreenDirection != vec2(0.0f))
            {
                rotationScreenDirection = Normalize(rotationScreenDirection);

                AssertRelease(!isNan(rotationScreenDirection));

                const float rotationSpeed = speedFactor * Radians(moverTemplate->CameraRotationMaxSpeed());
                moverModule.SetCurrentRotationSpeed(rotationSpeed);

                const quat currentOrientation = orientationModule->GetOrientation();
                quat newOrientation = angleAxis(rotationScreenDirection.x * rotationSpeed * deltaTime, vec3(0.f, 1.f, 0.f)) * currentOrientation;
                newOrientation = angleAxis(rotationScreenDirection.y * rotationSpeed * deltaTime, ((mat4)newOrientation).Column(0).xyz()) * newOrientation;

                orientationModule->SetOrientation(newOrientation);
                overrideWorldViewMatrix = true;
            }
            else
            {
                moverModule.SetCurrentRotationSpeed(0.f);
            }
        }

        if (FUp.Evaluate())
            movementCommand.y += speedFactor;

        if (FDown.Evaluate())
            movementCommand.y -= speedFactor;

        if (FLeft.Evaluate())
            movementCommand.x += speedFactor;

        if (FRight.Evaluate())
            movementCommand.x -= speedFactor;

        if (FMiddleMouseScroll.Evaluate())
        {
            vec2 mouseScroll = Input::GetMouseScrollDelta();

            float delta = mouseScroll.y;

            movementCommand.z += 10.0f * delta * speedFactor;
        }

        if (movementCommand != vec3(.0f))
        {
            const float incrementFactor = moverModule.CurrentSpeed() +
                  Clamp(moverTemplate->CameraAcceleration() * speedFactor * deltaTime, 0.f, moverTemplate->CameraMaxSpeed());

            vec3 movement = vec3(mat4_cast(orientationModule->GetOrientation()) * incrementFactor * vec4(movementCommand, 1.0f));
            positionModule->SetPosition3D(positionModule->GetPosition3D() + movement);

            overrideWorldViewMatrix = true;
        }

        if (overrideWorldViewMatrix)
        {
            Camera* camera = CameraManager::Instance().GetCamera(moverModule.CamId());
            const mat4 worldview = Translation(positionModule->GetPosition3D()) * mat4_cast(orientationModule->GetOrientation());
            camera->SetWorldViewMatrix(Invert(worldview));
        }
    }
}

} // namespace ECSEngine
