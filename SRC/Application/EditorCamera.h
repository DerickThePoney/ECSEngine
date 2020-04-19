#pragma once

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
    u32 FCameraId;
    std::weak_ptr<Camera> FCamera;
};
} // namespace ECSEngine