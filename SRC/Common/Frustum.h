#pragma once
#include "MemoryView.h"

namespace ECSEngine
{
namespace FrustumCorner
{
enum Type
{
    NEAR_TOP_LEFT,
    NEAR_TOP_RIGHT,
    NEAR_BOTTOM_LEFT,
    NEAR_BOTTOM_RIGHT,
    FAR_TOP_LEFT,
    FAR_TOP_RIGHT,
    FAR_BOTTOM_LEFT,
    FAR_BOTTOM_RIGHT
};
}

namespace FrustumPlane
{
enum Type
{
    NEAR_PLANE,
    FAR_PLANE,
    LEFT_PLANE,
    RIGHT_PLANE,
    TOP_PLANE,
    BOTTOM_PLANE
};
}

class Camera;
class FrustumCorners;
class Frustum
{
public:
    Frustum();

    void InitFromCamera(const Camera& parCamera, const float parAspectRatio);
    void InitFromMatrices(const glm::mat4& parViewWorldMatrix, const glm::mat4& parInverseProjectionMatrix);
    void InitFromCorners(const FrustumCorners& parCorners);
    MemoryView<const glm::vec4> GetPlanes() const;

private:
    std::array<glm::vec4, 6> FPlanes;
};

class FrustumCorners
{
public:
    FrustumCorners();

    void InitFromCamera(const Camera& parCamera, const float parAspectRatio);
    void InitFromMatrices(const glm::mat4& parViewWorldMatrix, const glm::mat4& parInverseProjectionMatrix);

    MemoryView<const glm::vec4> GetCorners() const;

private:
    std::array<glm::vec4, 8> FCorners;
};
} // namespace ECSEngine
