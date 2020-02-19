#pragma once
#include "Common/Singleton.h"

namespace ECSEngine
{
namespace Rendering
{

class Camera;
class CameraManager : public Singleton<CameraManager>
{
public:
    CameraManager();
    ~CameraManager();

    void CreateMainCamera(const glm::vec3& at);

private:
};
} // namespace Rendering
} // namespace ECSEngine