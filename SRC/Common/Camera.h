#pragma once

namespace ECSEngine
{

class Camera
{
public:
    Camera();
    Camera(const mat4& parWorldViewMatrix, const float parFov, const float parNearPlane, const float parFarPlane);

    void Init(const mat4& parWorldViewMatrix, const float parFov, const float parNearPlane, const float parFarPlane);

    const vec4& Position() const { return FWorldViewMatrix.Column(3); }
    const float Fov() const { return FFov; }
    const float Near() const { return FNearPlane; }
    const float Far() const { return FFarPlane; }
    const vec4& Forward() const { return FWorldViewMatrix.Column(2); }
    const vec4& Right() const { return FWorldViewMatrix.Column(0); }
    const vec4& Up() const { return FWorldViewMatrix.Column(1); }

    void SetPosition(const vec4& parPosition) { FWorldViewMatrix.SetColumn(3, parPosition); }
    void SetFov(const float parFov) { FFov = parFov; }
    void SetNear(const float parNear) { FNearPlane = parNear; }
    void SetFar(const float parFar) { FFarPlane = parFar; }
    void SetWorldViewMatrix(const mat4& parWorldViewMatrix) { FWorldViewMatrix = parWorldViewMatrix; }

    void Translate(const vec3& parTranslation);
    void Rotate(const mat4& parRotationMatrix);

    mat4 GetProjectionMatrix(const float parAspectRatio) const;
    mat4 GetWorldViewMatrix() const;

private:
    mat4 FWorldViewMatrix;

    float FFov;
    float FNearPlane;
    float FFarPlane;
};

} // namespace ECSEngine
