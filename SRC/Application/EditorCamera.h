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

private:
    KeyboardCommand FForward;
    KeyboardCommand FBackward;
    KeyboardCommand FLeft;
    KeyboardCommand FRight;
    KeyboardCommand FUp;
    KeyboardCommand FDown;

    u32 FCameraId;

    float FForwardSpeed = 100.0f;
    float FLateralSpeed = 50.0f;
    float FRotationSpeed = 45.0f;
};
} // namespace ECSEngine