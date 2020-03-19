#pragma once
#include "Camera.h"
#include "Singleton.h"

namespace ECSEngine
{

class CameraManager : public Singleton<CameraManager>
{
public:
    CameraManager();
    ~CameraManager();

    u32 CreateCamera();
    void DestroyCamera(u32 parCameraId);

    std::weak_ptr<Camera> GetCamera(u32 parCameraId);

private:
    static u32 sCameraId;

    std::vector<std::shared_ptr<Camera>> FCameras;
};
} // namespace ECSEngine