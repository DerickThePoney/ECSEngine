#pragma once
#include "Ray.h"

namespace ECSEngine
{
class Camera;
Ray GetCameraRayFromMouseInput(const Camera& parCamera, const float parAspectRatio, const glm::uvec2 parWindowSize, const glm::vec2 parMousePosition);
} // namespace ECSEngine
