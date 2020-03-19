#include "stdafx.h"

#include "Camera.h"

namespace ECSEngine
{

Camera::Camera()
    : FPosition(glm::vec3(0.0f))
    , FOrientation(glm::quat())
    , FFov(glm::radians(50.0f))
    , FFarPlane(150.0f)
    , FNearPlane(0.1f)
{
}

Camera::Camera(const glm::vec3& parPosition, const glm::quat& parOrientation, const float parFov, const float parNearPlane, const float parFarPlane)
    : FPosition(parPosition)
    , FOrientation(parOrientation)
    , FFov(parFov)
    , FFarPlane(parNearPlane)
    , FNearPlane(parFarPlane)
{
}

void Camera::Init(const glm::vec3& parPosition, const glm::quat& parOrientation, const float parFov, const float parNearPlane, const float parFarPlane)
{
    FPosition = parPosition;
    FOrientation = parOrientation;
    FFov = parFov;
    FFarPlane = parNearPlane;
    FNearPlane = parFarPlane;
}

glm::mat4 Camera::GetProjectionMatrix(const float parAspectRatio)
{
    return glm::perspective(FFov, parAspectRatio, FNearPlane, FFarPlane);
}

glm::mat4 Camera::GetWorldViewMatrix()
{
    return glm::translate(FPosition) * glm::mat4(FOrientation);
}

} // namespace ECSEngine