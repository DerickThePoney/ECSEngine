#pragma once
#include "Ray.h"

namespace ECSEngine
{
class Camera;
Ray3D GetCameraRayFromMouseInput(const Camera& parCamera, const float parAspectRatio, const uvec2 parWindowSize, const vec2 parMousePosition);
vec3 GetWorldPositionFromScreenPosition(const Camera& parCamera,
      const float parAspectRatio,
      const uvec2 parWindowSize,
      const vec2 parMousePosition,
      bool& outInWorld);
} // namespace ECSEngine
