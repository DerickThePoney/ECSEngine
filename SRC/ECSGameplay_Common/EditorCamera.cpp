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

    /*glm::mat4 localWorld = glm::identity<glm::mat4>();

    glm::vec3 f = glm::normalize(at - eye);
    glm::vec3 axis = glm::normalize(glm::cross(f, glm::vec3(0.f, 0.f, 1.f)));

    float angle = -glm::acos(glm::dot(f, glm::vec3(0.f, 0.f, 1.f)));

    localWorld = glm::rotate(angle, axis) * localWorld;
    localWorld = glm::translate(eye) * localWorld;

    glm::mat4 invlocalworld = glm::inverse(localWorld);

    glm::mat4 localWorld2 = glm::identity<glm::mat4>();
    localWorld2 = glm::translate(eye) * localWorld2;
    localWorld2 = glm::rotate(angle, axis) * localWorld2;

    glm::mat4 invlocalworld2 = glm::inverse(localWorld2);

    glm::mat4 invworldView = glm::inverse(worldWiewMatrix);

    glm::mat4 worldWiewMatrix2 = glm::lookAt(glm::vec3(localWorld[3]), glm::vec3(localWorld[3]) + glm::vec3(localWorld[2]), glm::vec3(0, 1.0f, 0.0f));*/

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

    bool overrideWorldViewMatrix = false;
    glm::vec3 movementCommand = glm::vec3(0.0f);
    glm::vec2 rotationScreenDirection = glm::vec2(0.0f);

    if (FMiddleMouseRotation.Evaluate())
    {
        rotationScreenDirection = Input::GetMousePositionDelta();

#ifdef PERFORM_SECURITY_CHECKS
        {
            std::stringstream sstr;
            sstr << "Command: " << rotationScreenDirection[0] << "\t" << rotationScreenDirection[1];
            LOG_INPUT(sstr.str());
        }
#endif

        if (rotationScreenDirection != glm::vec2(0.0f))
        {
            rotationScreenDirection = glm::normalize(rotationScreenDirection);
            AssertRelease(!glm::isNan(rotationScreenDirection));
            const glm::mat4 rotationMatrix = glm::rotate(rotationScreenDirection.y * glm::radians(FRotationSpeed) * deltaTime, glm::vec3(1.f, 0.f, 0.f)) *
                  glm::rotate(rotationScreenDirection.x * glm::radians(FRotationSpeed) * deltaTime, glm::vec3(0.f, 1.f, 0.f));

            const glm::mat4 positionMatrix = glm::translate(glm::vec3(FViewWorldMatrix[3]));

            FViewWorldMatrix = positionMatrix * rotationMatrix * glm::inverse(positionMatrix) * FViewWorldMatrix;
            overrideWorldViewMatrix = true;
        }
    }

    const bool slowMo = Input::IsCtrlDown();
    const float factor = (slowMo) ? 0.2f : 1.0f;
    if (FForward.Evaluate())
        movementCommand.z += factor;

    if (FBackward.Evaluate())
        movementCommand.z -= factor;

    if (FLeft.Evaluate())
        movementCommand.x += factor;

    if (FRight.Evaluate())
        movementCommand.x -= factor;

    if (FUp.Evaluate())
        movementCommand.y += factor;

    if (FDown.Evaluate())
        movementCommand.y -= factor;

    if (movementCommand != glm::vec3(.0f))
    {
#ifdef PERFORM_SECURITY_CHECKS
        std::stringstream sstr;
        sstr << "Command: " << movementCommand[0] << "\t" << movementCommand[1] << "\t" << movementCommand[2];
        LOG_INPUT(sstr.str());
#endif
        const float incrementFactor = FLateralSpeed * factor * deltaTime;
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