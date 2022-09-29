#include "stdafx.h"

#include "Camera.h"

namespace ECSEngine
{

Camera::Camera()
    : FWorldViewMatrix(mat4::Identity())
    , FFov(Radians(50.0f))
    , FFarPlane(150.0f)
    , FNearPlane(0.1f)
{
}

Camera::Camera(const mat4& parWorldViewMatrix, const float parFov, const float parNearPlane, const float parFarPlane)
    : FWorldViewMatrix(parWorldViewMatrix)
    , FFov(parFov)
    , FNearPlane(parNearPlane)
    , FFarPlane(parFarPlane)
{
}

void Camera::Init(const mat4& parWorldViewMatrix, const float parFov, const float parNearPlane, const float parFarPlane)
{
    FWorldViewMatrix = parWorldViewMatrix;
    FFov = parFov;
    FNearPlane = parNearPlane;
    FFarPlane = parFarPlane;
}

void Camera::Translate(const vec3& parTranslation)
{
    FWorldViewMatrix = Translation(parTranslation) * FWorldViewMatrix;
}

void Camera::Rotate(const mat4& parRotationMatrix)
{
    FWorldViewMatrix = Invert(Invert(FWorldViewMatrix) * parRotationMatrix);
}

mat4 Camera::GetProjectionMatrix(const float parAspectRatio) const
{
    return Perspective(FFov, parAspectRatio, FNearPlane, FFarPlane);
}

mat4 Camera::GetWorldViewMatrix() const
{
    return FWorldViewMatrix;
}

} // namespace ECSEngine
