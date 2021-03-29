#include "stdafx.h"

#include "Frustum.h"

#include "Camera.h"

namespace ECSEngine
{

FrustumCorners::FrustumCorners()
{
}

void FrustumCorners::InitFromCamera(const Camera& parCamera, const float parAspectRatio)
{
    const glm::mat4 viewWorld = glm::inverse(parCamera.GetWorldViewMatrix());
    const glm::mat4 invProj = glm::inverse(parCamera.GetProjectionMatrix(parAspectRatio));

    InitFromMatrices(viewWorld, invProj);
}

void FrustumCorners::InitFromMatrices(const glm::mat4& parViewWorldMatrix, const glm::mat4& parInverseProjectionMatrix)
{
    FCorners = {
        glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f), /*near_top_left*/
        glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), /*near_top_right*/
        glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f), /*near_bt_left*/
        glm::vec4(1.0f, -1.0f, 0.0f, 1.0f), /*near_bt_right*/
        glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), /*far_top_left*/
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), /*far_top_right*/
        glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f), /*far_bt_left*/
        glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), /*far_bt_right*/
    };

    forrange(i, 0, 8)
    {
        glm::vec4 worldVertex = parInverseProjectionMatrix * FCorners[i];
        worldVertex = worldVertex / worldVertex.w;
        FCorners[i] = parViewWorldMatrix * worldVertex;
    }
}

MemoryView<const glm::vec4> FrustumCorners::GetCorners() const
{
    return MemoryView<const glm::vec4>(FCorners.data(), (u32)FCorners.size());
}

Frustum::Frustum()
{
}

void Frustum::InitFromCamera(const Camera& parCamera, const float parAspectRatio)
{
    const glm::mat4 viewWorld = glm::inverse(parCamera.GetWorldViewMatrix());
    const glm::mat4 invProj = glm::inverse(parCamera.GetProjectionMatrix(parAspectRatio));

    InitFromMatrices(viewWorld, invProj);
}

void Frustum::InitFromMatrices(const glm::mat4& parViewWorldMatrix, const glm::mat4& parInverseProjectionMatrix)
{
    FrustumCorners corners;
    corners.InitFromMatrices(parViewWorldMatrix, parInverseProjectionMatrix);
    InitFromCorners(corners);
}

void Frustum::InitFromCorners(const FrustumCorners& parCorners)
{
    const MemoryView<const glm::vec4> corners = parCorners.GetCorners();
    FPlanes[FrustumPlane::NEAR_PLANE] = glm::vec4(glm::normalize(glm::cross(glm::vec3(corners[FrustumCorner::NEAR_TOP_LEFT] - corners[FrustumCorner::NEAR_TOP_RIGHT]),
                                                        glm::vec3(corners[FrustumCorner::NEAR_BOTTOM_LEFT] - corners[FrustumCorner::NEAR_TOP_RIGHT]))),
          0.f);
    FPlanes[FrustumPlane::NEAR_PLANE].w = -glm::dot(FPlanes[FrustumPlane::NEAR_PLANE], corners[FrustumCorner::NEAR_TOP_LEFT]);

    FPlanes[FrustumPlane::FAR_PLANE] = glm::vec4(glm::normalize(glm::cross(glm::vec3(corners[FrustumCorner::FAR_TOP_RIGHT] - corners[FrustumCorner::FAR_TOP_LEFT]),
                                                       glm::vec3(corners[FrustumCorner::FAR_BOTTOM_RIGHT] - corners[FrustumCorner::FAR_TOP_LEFT]))),
          0.f);
    FPlanes[FrustumPlane::FAR_PLANE].w = -glm::dot(FPlanes[FrustumPlane::FAR_PLANE], corners[FrustumCorner::FAR_TOP_LEFT]);

    FPlanes[FrustumPlane::LEFT_PLANE] = glm::vec4(glm::normalize(glm::cross(glm::vec3(corners[FrustumCorner::FAR_TOP_LEFT] - corners[FrustumCorner::NEAR_TOP_LEFT]),
                                                        glm::vec3(corners[FrustumCorner::FAR_BOTTOM_LEFT] - corners[FrustumCorner::NEAR_TOP_LEFT]))),
          0.0f);
    FPlanes[FrustumPlane::LEFT_PLANE].w = -glm::dot(FPlanes[FrustumPlane::LEFT_PLANE], corners[FrustumCorner::NEAR_TOP_LEFT]);

    FPlanes[FrustumPlane::RIGHT_PLANE] = glm::vec4(glm::normalize(glm::cross(glm::vec3(corners[FrustumCorner::FAR_BOTTOM_RIGHT] - corners[FrustumCorner::NEAR_TOP_RIGHT]),
                                                         glm::vec3(corners[FrustumCorner::FAR_TOP_RIGHT] - corners[FrustumCorner::NEAR_TOP_RIGHT]))),
          0.0f);
    FPlanes[FrustumPlane::RIGHT_PLANE].w = -glm::dot(FPlanes[FrustumPlane::RIGHT_PLANE], corners[FrustumCorner::NEAR_TOP_RIGHT]);

    FPlanes[FrustumPlane::TOP_PLANE] = glm::vec4(glm::normalize(glm::cross(glm::vec3(corners[FrustumCorner::NEAR_TOP_RIGHT] - corners[FrustumCorner::NEAR_TOP_LEFT]),
                                                       glm::vec3(corners[FrustumCorner::FAR_TOP_LEFT] - corners[FrustumCorner::NEAR_TOP_LEFT]))),
          0.0f);
    FPlanes[FrustumPlane::TOP_PLANE].w = -glm::dot(FPlanes[FrustumPlane::TOP_PLANE], corners[FrustumCorner::NEAR_TOP_LEFT]);

    FPlanes[FrustumPlane::BOTTOM_PLANE] = glm::vec4(glm::normalize(glm::cross(glm::vec3(corners[FrustumCorner::NEAR_BOTTOM_LEFT] - corners[FrustumCorner::NEAR_BOTTOM_RIGHT]),
                                                          glm::vec3(corners[FrustumCorner::FAR_BOTTOM_RIGHT] - corners[FrustumCorner::NEAR_BOTTOM_RIGHT]))),
          0.0f);
    FPlanes[FrustumPlane::BOTTOM_PLANE].w = -glm::dot(FPlanes[FrustumPlane::BOTTOM_PLANE], corners[FrustumCorner::NEAR_BOTTOM_RIGHT]);
}

MemoryView<const glm::vec4> Frustum::GetPlanes() const
{
    return MemoryView(FPlanes.data(), (u32)FPlanes.size());
}

} // namespace ECSEngine
