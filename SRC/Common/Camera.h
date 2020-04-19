#pragma once

namespace ECSEngine
{

class Camera
{
public:
    Camera();
    Camera(const glm::mat4& parWorldViewMatrix, const float parFov, const float parNearPlane, const float parFarPlane);

    void Init(const glm::mat4& parWorldViewMatrix, const float parFov, const float parNearPlane, const float parFarPlane);

    const glm::vec4& Position() const { return FWorldViewMatrix[3]; }
    const float Fov() const { return FFov; }
    const float Near() const { return FNearPlane; }
    const float Far() const { return FFarPlane; }

    void SetPosition(const glm::vec4& parPosition) { FWorldViewMatrix[3] = parPosition; }
    void SetFov(const float parFov) { FFov = parFov; }
    void SetNear(const float parNear) { FNearPlane = parNear; }
    void SetFar(const float parFar) { FFarPlane = parFar; }

    void Translate(const glm::vec3& parTranslation);

    glm::mat4 GetProjectionMatrix(const float parAspectRatio);
    glm::mat4 GetWorldViewMatrix();

private:
    glm::mat4 FWorldViewMatrix;

    float FFov;
    float FNearPlane;
    float FFarPlane;
};

} // namespace ECSEngine