#pragma once

namespace ECSEngine
{

class Camera
{
public:
    Camera();
    Camera(const glm::vec3& parPosition, const glm::quat& parOrientation, const float parFov, const float parNearPlane, const float parFarPlane);

    void Init(const glm::vec3& parPosition, const glm::quat& parOrientation, const float parFov, const float parNearPlane, const float parFarPlane);

    const glm::vec3 Position() const { return FPosition; }
    const glm::quat& Orientation() const { return FOrientation; }
    const float Fov() const { return FFov; }
    const float Near() const { return FNearPlane; }
    const float Far() const { return FFarPlane; }

    void SetPosition(const glm::vec3& parPosition) { FPosition = parPosition; }
    void SetOrientation(const glm::quat& parOrientation) { FOrientation = parOrientation; }
    void SetFov(const float parFov) { FFov = parFov; }
    void SetNear(const float parNear) { FNearPlane = parNear; }
    void SetFar(const float parFar) { FFarPlane = parFar; }

    glm::mat4 GetProjectionMatrix(const float parAspectRatio);
    glm::mat4 GetWorldViewMatrix();

private:
    glm::vec3 FPosition;
    glm::quat FOrientation;

    float FFov;
    float FNearPlane;
    float FFarPlane;
};

} // namespace ECSEngine