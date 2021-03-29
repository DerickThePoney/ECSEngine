#pragma once
#include "Ray.h"

namespace ECSEngine
{
class Camera;
Ray3D GetCameraRayFromMouseInput(const Camera& parCamera, const float parAspectRatio, const glm::uvec2 parWindowSize, const glm::vec2 parMousePosition);
glm::vec3 GetWorldPositionFromScreenPosition(const Camera& parCamera,
      const float parAspectRatio,
      const glm::uvec2 parWindowSize,
      const glm::vec2 parMousePosition,
      bool& outInWorld);
} // namespace ECSEngine
