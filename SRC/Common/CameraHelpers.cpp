#include "stdafx.h"

#include "CameraHelpers.h"

#include "Camera.h"

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

} // namespace ECSEngine
