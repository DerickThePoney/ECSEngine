#include "stdafx.h"

#include "CameraMoverSystem.h"

#include "CameraMoverModule.h"
#include "Common/CameraManager.h"
#include "Common/GLMHelpers.h"
#include "Common/Logger.h"
#include "Common/TimeManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "OrientationModule.h"
#include "PositionModule.h"

namespace ECSEngine
{

CameraMoverSystem::CameraMoverSystem()
{
    RegisterDepency<CameraMoverModule>(Worlds::CAMERA);
    RegisterDepency<PositionModule>(Worlds::CAMERA);
    RegisterDepency<OrientationModule>(Worlds::CAMERA);

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

    ModuleAccessor<CameraMoverModule> cameraMoverAccessor(Worlds::CAMERA);
    ModuleAccessor<PositionModule> positionAccessor(Worlds::CAMERA);
    ModuleAccessor<OrientationModule> orientationAccessor(Worlds::CAMERA);
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
        glm::vec3 movementCommand = glm::vec3(0.0f);
        glm::vec2 rotationScreenDirection = glm::vec2(0.0f);

        rotationScreenDirection = Input::GetMousePositionDelta();
        glm::mat4 newViewWorldMatrix = glm::identity<glm::mat4>();

        if (FMiddleMouseRotation.Evaluate())
        {
            if (rotationScreenDirection != glm::vec2(0.0f))
            {
                rotationScreenDirection = glm::normalize(rotationScreenDirection);

                AssertRelease(!glm::isNan(rotationScreenDirection));

                const float rotationSpeed = speedFactor * glm::radians(moverTemplate->CameraRotationMaxSpeed());
                moverModule.SetCurrentRotationSpeed(rotationSpeed);

                const glm::quat currentOrientation = orientationModule->GetOrientation();
                glm::quat newOrientation = glm::angleAxis(rotationScreenDirection.x * rotationSpeed * deltaTime, glm::vec3(0.f, 1.f, 0.f)) * currentOrientation;
                newOrientation = glm::angleAxis(rotationScreenDirection.y * rotationSpeed * deltaTime, glm::vec3(glm::mat4_cast(newOrientation)[0])) * newOrientation;

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
            glm::vec2 mouseScroll = Input::GetMouseScrollDelta();

            float delta = mouseScroll.y;

            movementCommand.z += 10.0f * delta * speedFactor;
        }

        if (movementCommand != glm::vec3(.0f))
        {
            const float incrementFactor = moverModule.CurrentSpeed() +
                  glm::clamp(moverTemplate->CameraAcceleration() * speedFactor * deltaTime, 0.f, moverTemplate->CameraMaxSpeed());

            glm::vec3 movement = glm::vec3(glm::mat4_cast(orientationModule->GetOrientation()) * incrementFactor * glm::vec4(movementCommand, 1.0f));
            positionModule->SetPosition3D(positionModule->GetPosition3D() + movement);

            overrideWorldViewMatrix = true;
        }

        if (overrideWorldViewMatrix)
        {
            Camera* camera = CameraManager::Instance().GetCamera(moverModule.CamId());
            const glm::mat4 worldview = glm::translate(positionModule->GetPosition3D()) * glm::mat4_cast(orientationModule->GetOrientation());
            camera->SetWorldViewMatrix(glm::inverse(worldview));
        }
    }
}

} // namespace ECSEngine
