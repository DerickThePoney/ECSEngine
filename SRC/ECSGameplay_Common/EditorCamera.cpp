#include "stdafx.h"

#include "EditorCamera.h"

#include "Common/Camera.h"
#include "Common/CameraManager.h"
#include "Common/GLMHelpers.h"
#include "Common/InputManager.h"
#include "Common/Logger.h"
#include "Common/TimeManager.h"

namespace ECSEngine
{

EditorCamera::EditorCamera()
    : FCameraId(-1)
{
    FForward.FKeyboardKey = InputKeyNames::INPUT_KEY_W;
    FForward.FInputType = EInputType::REPEATED;

    FBackward.FKeyboardKey = InputKeyNames::INPUT_KEY_S;
    FBackward.FInputType = EInputType::REPEATED;

    FRight.FKeyboardKey = InputKeyNames::INPUT_KEY_A;
    FRight.FInputType = EInputType::REPEATED;

    FLeft.FKeyboardKey = InputKeyNames::INPUT_KEY_D;
    FLeft.FInputType = EInputType::REPEATED;

    FUp.FKeyboardKey = InputKeyNames::INPUT_KEY_W;
    FUp.FInputType = EInputType::REPEATED;
    FUp.FShift = true;

    FDown.FKeyboardKey = InputKeyNames::INPUT_KEY_S;
    FDown.FInputType = EInputType::REPEATED;
    FDown.FShift = true;

    FMiddleMouseRotation.FMouseButton = MouseButtons::MOUSE_BUTTON_3;
    FMiddleMouseRotation.FInputType = EInputType::REPEATED;
}

EditorCamera::~EditorCamera()
{
}

void EditorCamera::Initialise()
{
    const glm::vec3 at = { 0.0f, 0.0f, 0.0f };
    const glm::vec3 eye = { 0.0f, 10.0f, -10.0f };

    glm::mat4 worldWiewMatrix = glm::lookAt(eye, at, glm::vec3(0, 1.0f, 0.0f));

    FCameraId = CameraManager::Instance().CreateCameraIFN("EditorCamera");
    AssertRelease(FCameraId != -1);
    Camera* camera = CameraManager::Instance().GetCamera(FCameraId);
    AssertRelease(camera != nullptr);
    camera->Init(worldWiewMatrix, glm::radians(60.0f), 1.f, 200.0f);
}

void EditorCamera::Update()
{
    Camera* camera = CameraManager::Instance().GetCamera(FCameraId);
    AssertRelease(camera != nullptr);

    const float deltaTime = TimeManager::FrameDeltaTime();

    glm::vec3 movementCommand = glm::vec3(0.0f);
    glm::vec2 rotationScreenDirection = glm::vec2(0.0f);
    const bool slowMo = Input::IsCtrlDown();
    const float factor = 1.0f;
    if (FForward.Evaluate())
        movementCommand.x -= factor;

    if (FBackward.Evaluate())
        movementCommand.x += factor;

    if (FLeft.Evaluate())
        movementCommand.y -= factor;

    if (FRight.Evaluate())
        movementCommand.y += factor;

    if (FUp.Evaluate())
        movementCommand.z -= factor;

    if (FDown.Evaluate())
        movementCommand.z += factor;

    if (movementCommand != glm::vec3(.0f))
    {
#ifdef PERFORM_SECURITY_CHECKS
        std::stringstream sstr;
        sstr << "Command: " << movementCommand[0] << "\t" << movementCommand[1] << "\t" << movementCommand[2];
        LOG_INPUT(sstr.str());
#endif
        camera->Translate(glm::vec3(movementCommand.y * FLateralSpeed * deltaTime, movementCommand.z * FLateralSpeed * deltaTime, movementCommand.x * FForwardSpeed * deltaTime));
    }

    //    if (FMiddleMouseRotation.Evaluate())
    //    {
    //        rotationScreenDirection = Input::GetMousePositionDelta();
    //
    //#ifdef PERFORM_SECURITY_CHECKS
    //        std::stringstream sstr;
    //        sstr << "Command: " << rotationScreenDirection[0] << "\t" << rotationScreenDirection[1];
    //        LOG_INPUT(sstr.str());
    //#endif
    //
    //        if (rotationScreenDirection != glm::vec2(0.0f))
    //        {
    //            rotationScreenDirection = glm::normalize(rotationScreenDirection);
    //            AssertRelease(!glm::isNan(rotationScreenDirection));
    //            rotationScreenDirection.y = -rotationScreenDirection.y;
    //            const glm::vec3 rotationViewDirection = glm::normalize(
    //                  glm::vec3(1.0f, 0.0f, 0.0f) * rotationScreenDirection.x + glm::vec3(0.0f, 1.0f, 0.0f) * rotationScreenDirection.y);
    //            const glm::vec3 rotationAxis = glm::cross(rotationViewDirection, glm::vec3(0.0f, 0.0f, 1.0f));
    //
    //            const glm::mat4 rotationMatrix = glm::axisAngleMatrix(rotationAxis, glm::radians(FRotationSpeed) * deltaTime);
    //
    //            camera->Rotate(rotationMatrix);
    //
    //            /*const glm::vec3 newForward = rotationMatrix * camera->Forward();
    //
    //            const glm::mat4 worldWiewMatrix = glm::lookAt(glm::vec3(camera->Position()), glm::vec3(camera->Position()) + newForward, glm::vec3(0, 0, 1.0f));
    //
    //            camera->SetWorldViewMatrix(worldWiewMatrix);*/
    //        }
    //    }
}

void EditorCamera::Shutdown()
{
    CameraManager::Instance().DestroyCamera(FCameraId);
}

} // namespace ECSEngine