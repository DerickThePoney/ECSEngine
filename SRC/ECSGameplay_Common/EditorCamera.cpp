#include "stdafx.h"

#include "EditorCamera.h"

#include "Application/PropertyDrawer.h"
#include "Common/Camera.h"
#include "Common/CameraManager.h"
//#include "Common/GLMHelpers.h"
#include "Common/InputManager.h"
#include "Common/Logger.h"
#include "Common/TimeManager.h"

namespace ECSEngine
{

EditorCamera::EditorCamera()
    : FCameraId(-1)
    , FViewWorldMatrix(mat4::Identity())
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
    const vec3 at(0.0f, 0.0f, 0.0f);
    const vec3 eye(0.0f, 10.0f, -10.0f);

    mat4 worldWiewMatrix = LookAt(eye, at, vec3(0, 1.0f, 0.0f));
    FViewWorldMatrix = Invert(worldWiewMatrix);

    FCameraId = CameraManager::Instance().CreateCameraIFN("EditorCamera");
    AssertRelease(FCameraId != -1);
    Camera* camera = CameraManager::Instance().GetCamera(FCameraId);
    AssertRelease(camera != nullptr);
    camera->Init(worldWiewMatrix, Radians(60.0f), 1.f, 500.0f);
}

void EditorCamera::Update()
{
    Camera* camera = CameraManager::Instance().GetCamera(FCameraId);
    AssertRelease(camera != nullptr);

    const float deltaTime = TimeManager::FrameDeltaTime();

    const bool slowMo = Input::IsCtrlDown();
    const float speedFactor = (slowMo) ? 0.2f : 1.0f;

    bool overrideWorldViewMatrix = false;
    vec3 movementCommand = vec3(0.0f);
    vec2 rotationScreenDirection = vec2(0.0f);

    rotationScreenDirection = Input::GetMousePositionDelta();

    if (FMiddleMouseRotation.Evaluate())
    {
        if (rotationScreenDirection != vec2(0.0f))
        {
            rotationScreenDirection = Normalize(rotationScreenDirection);

            AssertRelease(!IsNan(rotationScreenDirection));

            const mat4 positionMatrix = Translation(vec3(FViewWorldMatrix.Column(3).xyz()));
            const float rotationSpeed = Radians(speedFactor * FRotationSpeed);
            mat4 newWorldViewNoTranslation = Invert(positionMatrix) * FViewWorldMatrix;
            newWorldViewNoTranslation = Rotation(rotationScreenDirection.x * rotationSpeed * deltaTime, vec3(0.f, 1.f, 0.f)) * newWorldViewNoTranslation;
            newWorldViewNoTranslation = Rotation(rotationScreenDirection.y * rotationSpeed * deltaTime, vec3(newWorldViewNoTranslation.Column(1).xyz())) *
                  newWorldViewNoTranslation;

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

    if (movementCommand != vec3(.0f))
    {
        const float incrementFactor = FLateralSpeed * speedFactor * deltaTime;
        FViewWorldMatrix = FViewWorldMatrix * Translation(movementCommand * incrementFactor);
        overrideWorldViewMatrix = true;
    }

    if (overrideWorldViewMatrix)
        camera->SetWorldViewMatrix(Invert(FViewWorldMatrix));
}

void EditorCamera::Shutdown()
{
    CameraManager::Instance().DestroyCamera(FCameraId);
}

void EditorCamera::EditorWindow(bool* parOpen)
{
    Camera* camera = CameraManager::Instance().GetCamera(FCameraId);
    AssertRelease(camera != nullptr);

    float fov = camera->Fov();
    float nearPlane = camera->Near();
    float farPlane = camera->Far();
    ImGui::Begin("Editor Camera Parameters", parOpen);
    ImGui::Text("Camera Parameters");
    EDITOR_PROPERTY_ANGLE("FoV", fov, 0, 180);
    EDITOR_PROPERTY_WITH_LIMITS("Near plane", nearPlane, 0.01f, farPlane - 0.1f);
    EDITOR_PROPERTY_WITH_LIMITS("Far plane", farPlane, nearPlane + 0.2f, 1500.0f);

    camera->SetFov(fov);
    camera->SetNear(nearPlane);
    camera->SetFar(farPlane);

    ImGui::Separator();
    ImGui::Text("Camera Movement");
    EDITOR_PROPERTY_WITH_LIMITS("Forward speed", FForwardSpeed, 10.0f, 1000.0f);
    EDITOR_PROPERTY_WITH_LIMITS("Lateral speed", FLateralSpeed, 10.0f, 1000.0f);
    EDITOR_PROPERTY_WITH_LIMITS("Rotation speed", FRotationSpeed, 10.0f, 1000.0f);
    ImGui::End();
}

} // namespace ECSEngine
