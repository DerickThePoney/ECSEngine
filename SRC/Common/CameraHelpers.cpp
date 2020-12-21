#include "stdafx.h"

#include "CameraHelpers.h"

#include "Camera.h"
#include "IntersectionRoutines.h"
#include "Plane.h"

namespace ECSEngine
{

Ray3D GetCameraRayFromMouseInput(const Camera& parCamera, const float parAspectRatio, const glm::uvec2 parWindowSize, const glm::vec2 parMousePosition)
{
    const glm::mat4 projMatrix = parCamera.GetProjectionMatrix(parAspectRatio);
    const glm::mat4 invProjectionMatrix = glm::inverse(projMatrix);
    const glm::mat4 viewWorldMatrix = glm::inverse(parCamera.GetWorldViewMatrix());

    const float mouseXNDC = (parMousePosition.x / (float)parWindowSize.x) * 2.0f - 1.0f;
    const float mouseYNDC = ((parWindowSize.y - parMousePosition.y) / (float)parWindowSize.y) * 2.0f - 1.0f;

    const glm::vec4 pickEyeH = invProjectionMatrix * glm::vec4(mouseXNDC, mouseYNDC, 0.0f, 1.0f);
    const glm::vec4 pickAtH = invProjectionMatrix * glm::vec4(mouseXNDC, mouseYNDC, 1.0f, 1.0f);
    const glm::vec3 pickEye = viewWorldMatrix * pickEyeH / pickEyeH.w;
    const glm::vec3 pickAt = viewWorldMatrix * pickAtH / pickAtH.w;

    return Ray3D(pickEye, glm::normalize(pickAt - pickEye));
}

glm::vec3 GetWorldPositionFromScreenPosition(const Camera& parCamera,
      const float parAspectRatio,
      const glm::uvec2 parWindowSize,
      const glm::vec2 parMousePosition,
      bool& outInWorld)
{
    Ray3D r = GetCameraRayFromMouseInput(parCamera, parAspectRatio, parWindowSize, parMousePosition);
    Plane p{ glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f) };

    float inter = -1.f;
    outInWorld = false;
    if (Intersection::RayPlaneIntersection3D(r, p, inter))
    {
        const glm::vec3 posInter = r.FOrigin + r.FDirection * inter;
        outInWorld = true;
        return posInter;
    }
    return glm::vec3(0.f);
}

} // namespace ECSEngine
