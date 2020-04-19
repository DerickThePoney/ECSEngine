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

u32 CameraManager::CreateCameraIFN(const std::string& parName)
{
    auto itFind = FCameraNameToCameraId.find(parName);

    if (itFind != FCameraNameToCameraId.end())
        return itFind->second;

    FCameras.push_back(std::shared_ptr<Camera>(new Camera()));
    const u32 camId = sCameraId++;

    FCameraNameToCameraId[parName] = camId;
    return camId;
}

void CameraManager::DestroyCamera(u32 parCameraId)
{
    AssertRelease(FCameras.size() > parCameraId);
    FCameras[parCameraId] = nullptr;
}

Camera* CameraManager::GetCamera(u32 parCameraId)
{
    AssertRelease(FCameras.size() > parCameraId);
    AssertRelease(FCameras[parCameraId] != nullptr);
    return FCameras[parCameraId].get();
}

Camera* CameraManager::GetCamera(const std::string& parName)
{
    AlwaysCheckedAssertMsg(FCameraNameToCameraId.find(parName) != FCameraNameToCameraId.end(), "A camera can't be found!");
    const u32 camId = FCameraNameToCameraId[parName];
    return GetCamera(camId);
}

u32 CameraManager::sCameraId = 0;

} // namespace ECSEngine