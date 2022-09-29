#pragma once
#include "Common/InputCommands.h"

namespace ECSEngine
{
class Camera;
class EditorCamera
{
public:
    EditorCamera();
    ~EditorCamera();

    void Initialise();
    void Update();
    void Shutdown();

    void EditorWindow(bool* parOpen);

private:
    mat4 FViewWorldMatrix;

    KeyboardCommand FForward;
    KeyboardCommand FBackward;
    KeyboardCommand FLeft;
    KeyboardCommand FRight;
    KeyboardCommand FUp;
    KeyboardCommand FDown;

    MouseButtonCommand FMiddleMouseRotation;

    u32 FCameraId;

    float FForwardSpeed = 100.0f;
    float FLateralSpeed = 50.0f;
    float FRotationSpeed = 200.0f;
};
} // namespace ECSEngine
