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

    u32 CreateCameraIFN(const std::string& parName);
    void DestroyCamera(u32 parCameraId);

    Camera* GetCamera(const std::string& parName);
    Camera* GetCamera(u32 parCameraId);

private:
    static u32 sCameraId;

    std::vector<std::shared_ptr<Camera>> FCameras;
    std::unordered_map<std::string, u32> FCameraNameToCameraId;
};
} // namespace ECSEngine
