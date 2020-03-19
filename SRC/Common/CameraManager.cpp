#include "stdafx.h"

#include "CameraManager.h"

namespace ECSEngine
{

CameraManager::CameraManager()
    : Singleton()
{
}

CameraManager::~CameraManager()
{
    forrange(i, 0, FCameras.size()) FCameras[i] = nullptr;
}

u32 CameraManager::CreateCamera()
{
    FCameras.push_back(std::shared_ptr<Camera>(new Camera()));
    return sCameraId++;
}

void CameraManager::DestroyCamera(u32 parCameraId)
{
    AssertRelease(FCameras.size() > parCameraId);
    AssertRelease(FCameras[parCameraId] != nullptr);
    FCameras[parCameraId] = nullptr;
}

std::weak_ptr<Camera> CameraManager::GetCamera(u32 parCameraId)
{
    AssertRelease(FCameras.size() > parCameraId);
    AssertRelease(FCameras[parCameraId] != nullptr);
    return FCameras[parCameraId];
}

u32 CameraManager::sCameraId = 0;

} // namespace ECSEngine