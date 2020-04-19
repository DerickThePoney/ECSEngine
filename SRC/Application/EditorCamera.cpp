#include "stdafx.h"

#include "EditorCamera.h"

#include "Common/Camera.h"
#include "Common/CameraManager.h"

namespace ECSEngine
{

EditorCamera::EditorCamera()
{
}

EditorCamera::~EditorCamera()
{
}

void EditorCamera::Initialise()
{
    const glm::vec3 at = { 0.0f, 0.0f, 1.0f };
    const glm::vec3 eye = { 0.0f, 100.f, 0.0f };

    glm::mat4 worldWiewMatrix = glm::lookAt(eye, at, glm::vec3(0, 0, 1.0f));

    FCameraId = CameraManager::Instance().CreateCameraIFN("EditorCamera");
    FCamera = CameraManager::Instance().GetCamera(FCameraId);
    AlwaysCheckedAssert(!FCamera.expired());
    std::shared_ptr<Camera> cshared = FCamera.lock();
    cshared->Init(worldWiewMatrix, glm::radians(60.0f), 0.1f, 1500.0f);
}

void EditorCamera::Update()
{
}

void EditorCamera::Shutdown()
{
    if (!FCamera.expired())
        CameraManager::Instance().DestroyCamera(FCameraId);
}

} // namespace ECSEngine