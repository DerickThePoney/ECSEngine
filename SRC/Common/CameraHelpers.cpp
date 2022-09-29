#include "stdafx.h"

#include "CameraHelpers.h"

#include "Camera.h"
#include "IntersectionRoutines.h"
#include "Plane.h"

namespace ECSEngine
{

Ray3D GetCameraRayFromMouseInput(const Camera& parCamera, const float parAspectRatio, const uvec2 parWindowSize, const vec2 parMousePosition)
{
    const mat4 projMatrix = parCamera.GetProjectionMatrix(parAspectRatio);
    const mat4 invProjectionMatrix = Invert(projMatrix);
    const mat4 viewWorldMatrix = Invert(parCamera.GetWorldViewMatrix());

    const float mouseXNDC = (parMousePosition.x / (float)parWindowSize.x) * 2.0f - 1.0f;
    const float mouseYNDC = ((parWindowSize.y - parMousePosition.y) / (float)parWindowSize.y) * 2.0f - 1.0f;

    const vec4 pickEyeH = invProjectionMatrix * vec4(mouseXNDC, mouseYNDC, 0.0f, 1.0f);
    const vec4 pickAtH = invProjectionMatrix * vec4(mouseXNDC, mouseYNDC, 1.0f, 1.0f);
    const vec3 pickEye = (viewWorldMatrix * pickEyeH / pickEyeH.w).xyz();
    const vec3 pickAt = (viewWorldMatrix * pickAtH / pickAtH.w).xyz();

    return Ray3D(pickEye, Normalize(pickAt - pickEye));
}

vec3 GetWorldPositionFromScreenPosition(const Camera& parCamera,
      const float parAspectRatio,
      const uvec2 parWindowSize,
      const vec2 parMousePosition,
      bool& outInWorld)
{
    Ray3D r = GetCameraRayFromMouseInput(parCamera, parAspectRatio, parWindowSize, parMousePosition);
    Plane p{ vec3(0.f), vec3(0.f, 1.f, 0.f) };

    float inter = -1.f;
    outInWorld = false;
    if (Intersection::RayPlaneIntersection3D(r, p, inter))
    {
        const vec3 posInter = r.FOrigin + r.FDirection * inter;
        outInWorld = true;
        return posInter;
    }
    return vec3(0.f);
}

} // namespace ECSEngine
