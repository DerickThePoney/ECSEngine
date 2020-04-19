#include "stdafx.h"

#include "EditorCamera.h"

#include "Common/Camera.h"
#include "Common/CameraManager.h"
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
}

EditorCamera::~EditorCamera()
{
}

void EditorCamera::Initialise()
{
    const glm::vec3 at = { 0.0f, 0.0f, 0.0f };
    const glm::vec3 eye = { 0.0f, 100.f, 0.0f };

    glm::mat4 worldWiewMatrix = glm::lookAt(eye, at, glm::vec3(0, 0, 1.0f));

    FCameraId = CameraManager::Instance().CreateCameraIFN("EditorCamera");
    AssertRelease(FCameraId != -1);
    Camera* camera = CameraManager::Instance().GetCamera(FCameraId);
    AssertRelease(camera != nullptr);
    camera->Init(worldWiewMatrix, glm::radians(60.0f), 0.1f, 1500.0f);
}

void EditorCamera::Update()
{
    Camera* camera = CameraManager::Instance().GetCamera(FCameraId);
    AssertRelease(camera != nullptr);

    glm::vec3 movementCommand = glm::vec3(0.0f);

    if (FForward.Evaluate())
        movementCommand.x -= 1;

    if (FBackward.Evaluate())
        movementCommand.x += 1;

    if (FLeft.Evaluate())
        movementCommand.y -= 1;

    if (FRight.Evaluate())
        movementCommand.y += 1;

    if (FUp.Evaluate())
        movementCommand.z -= 1;

    if (FDown.Evaluate())
        movementCommand.z += 1;

    if (movementCommand != glm::vec3(.0f))
    {
#ifdef PERFORM_SECURITY_CHECKS
        std::stringstream sstr;
        sstr << "Command: " << movementCommand[0] << "\t" << movementCommand[1] << "\t" << movementCommand[2];
        LOG_INPUT(sstr.str());
#endif

        float deltaTime = TimeManager::FrameDeltaTime();
        camera->Translate(glm::vec3(movementCommand.y * FLateralSpeed * deltaTime, movementCommand.z * FLateralSpeed * deltaTime, movementCommand.x * FForwardSpeed * deltaTime));
    }
}

void EditorCamera::Shutdown()
{
    CameraManager::Instance().DestroyCamera(FCameraId);
}

} // namespace ECSEngine