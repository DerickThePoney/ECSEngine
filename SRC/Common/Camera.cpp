#include "stdafx.h"

#include "Camera.h"

namespace ECSEngine
{

Camera::Camera()
    : FWorldViewMatrix(glm::identity<glm::mat4>())
    , FFov(glm::radians(50.0f))
    , FFarPlane(150.0f)
    , FNearPlane(0.1f)
{
}

Camera::Camera(const glm::mat4& parWorldViewMatrix, const float parFov, const float parNearPlane, const float parFarPlane)
    : FWorldViewMatrix(parWorldViewMatrix)
    , FFov(parFov)
    , FNearPlane(parNearPlane)
    , FFarPlane(parFarPlane)
{
}

void Camera::Init(const glm::mat4& parWorldViewMatrix, const float parFov, const float parNearPlane, const float parFarPlane)
{
    FWorldViewMatrix = parWorldViewMatrix;
    FFov = parFov;
    FFarPlane = parNearPlane;
    FNearPlane = parFarPlane;
}

void Camera::Translate(const glm::vec3& parTranslation)
{
    FWorldViewMatrix = glm::translate(parTranslation) * FWorldViewMatrix;
}

void Camera::Rotate(const glm::mat4& parRotationMatrix)
{
    FWorldViewMatrix = glm::inverse(glm::inverse(FWorldViewMatrix) * parRotationMatrix);
}

glm::mat4 Camera::GetProjectionMatrix(const float parAspectRatio) const
{
    return glm::perspective(FFov, parAspectRatio, FNearPlane, FFarPlane);
}

glm::mat4 Camera::GetWorldViewMatrix() const
{
    return FWorldViewMatrix;
}

} // namespace ECSEngine