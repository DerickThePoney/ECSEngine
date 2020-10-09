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
    , FViewWorldMatrix(glm::identity<glm::mat4>())
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
    FViewWorldMatrix = glm::inverse(worldWiewMatrix);

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

    const bool slowMo = Input::IsCtrlDown();
    const float speedFactor = (slowMo) ? 0.2f : 1.0f;

    bool overrideWorldViewMatrix = false;
    glm::vec3 movementCommand = glm::vec3(0.0f);
    glm::vec2 rotationScreenDirection = glm::vec2(0.0f);

    rotationScreenDirection = Input::GetMousePositionDelta();

    if (FMiddleMouseRotation.Evaluate())
    {
        if (rotationScreenDirection != glm::vec2(0.0f))
        {
            rotationScreenDirection = glm::normalize(rotationScreenDirection);

            AssertRelease(!glm::isNan(rotationScreenDirection));

            const glm::mat4 positionMatrix = glm::translate(glm::vec3(FViewWorldMatrix[3]));
            const float rotationSpeed = glm::radians(speedFactor * FRotationSpeed);
            glm::mat4 newWorldViewNoTranslation = glm::inverse(positionMatrix) * FViewWorldMatrix;
            newWorldViewNoTranslation = glm::rotate(rotationScreenDirection.x * rotationSpeed * deltaTime, glm::vec3(0.f, 1.f, 0.f)) * newWorldViewNoTranslation;
            newWorldViewNoTranslation = glm::rotate(rotationScreenDirection.y * rotationSpeed * deltaTime, glm::vec3(newWorldViewNoTranslation[0])) * newWorldViewNoTranslation;

            FViewWorldMatrix = positionMatrix * newWorldViewNoTranslation;
            overrideWorldViewMatrix = true;
        }
    }

    if (FForward.Evaluate())
        movementCommand.z += speedFactor;

    if (FBackward.Evaluate())
        movementCommand.z -= speedFactor;

    if (FLeft.Evaluate())
        movementCommand.x += speedFactor;

    if (FRight.Evaluate())
        movementCommand.x -= speedFactor;

    if (FUp.Evaluate())
        movementCommand.y += speedFactor;

    if (FDown.Evaluate())
        movementCommand.y -= speedFactor;

    if (movementCommand != glm::vec3(.0f))
    {
        const float incrementFactor = FLateralSpeed * speedFactor * deltaTime;
        FViewWorldMatrix = FViewWorldMatrix * glm::translate(movementCommand * incrementFactor);
        overrideWorldViewMatrix = true;
    }

    if (overrideWorldViewMatrix)
        camera->SetWorldViewMatrix(glm::inverse(FViewWorldMatrix));
}

void EditorCamera::Shutdown()
{
    CameraManager::Instance().DestroyCamera(FCameraId);
}

} // namespace ECSEngine