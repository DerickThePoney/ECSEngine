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
    const mat4 viewWorld = Invert(parCamera.GetWorldViewMatrix());
    const mat4 invProj = Invert(parCamera.GetProjectionMatrix(parAspectRatio));

    InitFromMatrices(viewWorld, invProj);
}

void FrustumCorners::InitFromMatrices(const mat4& parViewWorldMatrix, const mat4& parInverseProjectionMatrix)
{
    FCorners = {
        vec4(-1.0f, 1.0f, 0.0f, 1.0f), /*near_top_left*/
        vec4(1.0f, 1.0f, 0.0f, 1.0f), /*near_top_right*/
        vec4(-1.0f, -1.0f, 0.0f, 1.0f), /*near_bt_left*/
        vec4(1.0f, -1.0f, 0.0f, 1.0f), /*near_bt_right*/
        vec4(-1.0f, 1.0f, 1.0f, 1.0f), /*far_top_left*/
        vec4(1.0f, 1.0f, 1.0f, 1.0f), /*far_top_right*/
        vec4(-1.0f, -1.0f, 1.0f, 1.0f), /*far_bt_left*/
        vec4(1.0f, -1.0f, 1.0f, 1.0f), /*far_bt_right*/
    };

    forrange(i, 0, 8)
    {
        vec4 worldVertex = parInverseProjectionMatrix * FCorners[i];
        worldVertex = worldVertex / worldVertex.w;
        FCorners[i] = parViewWorldMatrix * worldVertex;
    }
}

MemoryView<const vec4> FrustumCorners::GetCorners() const
{
    return MemoryView<const vec4>(FCorners.data(), (u32)FCorners.size());
}

Frustum::Frustum()
{
}

void Frustum::InitFromCamera(const Camera& parCamera, const float parAspectRatio)
{
    const mat4 viewWorld = Invert(parCamera.GetWorldViewMatrix());
    const mat4 invProj = Invert(parCamera.GetProjectionMatrix(parAspectRatio));

    InitFromMatrices(viewWorld, invProj);
}

void Frustum::InitFromMatrices(const mat4& parViewWorldMatrix, const mat4& parInverseProjectionMatrix)
{
    FrustumCorners corners;
    corners.InitFromMatrices(parViewWorldMatrix, parInverseProjectionMatrix);
    InitFromCorners(corners);
}

void Frustum::InitFromCorners(const FrustumCorners& parCorners)
{
    const MemoryView<const vec4> corners = parCorners.GetCorners();
    FPlanes[FrustumPlane::NEAR_PLANE] = vec4(Normalize(Cross((corners[FrustumCorner::NEAR_TOP_LEFT] - corners[FrustumCorner::NEAR_TOP_RIGHT]).xyz(),
                                                   (corners[FrustumCorner::NEAR_BOTTOM_LEFT] - corners[FrustumCorner::NEAR_TOP_RIGHT]).xyz())),
          0.f);
    FPlanes[FrustumPlane::NEAR_PLANE].w = -Dot(FPlanes[FrustumPlane::NEAR_PLANE], corners[FrustumCorner::NEAR_TOP_LEFT]);

    FPlanes[FrustumPlane::FAR_PLANE] = vec4(Normalize(Cross((corners[FrustumCorner::FAR_TOP_RIGHT] - corners[FrustumCorner::FAR_TOP_LEFT]).xyz(),
                                                  (corners[FrustumCorner::FAR_BOTTOM_RIGHT] - corners[FrustumCorner::FAR_TOP_LEFT]).xyz())),
          0.f);
    FPlanes[FrustumPlane::FAR_PLANE].w = -Dot(FPlanes[FrustumPlane::FAR_PLANE], corners[FrustumCorner::FAR_TOP_LEFT]);

    FPlanes[FrustumPlane::LEFT_PLANE] = vec4(Normalize(Cross((corners[FrustumCorner::FAR_TOP_LEFT] - corners[FrustumCorner::NEAR_TOP_LEFT]).xyz(),
                                                   (corners[FrustumCorner::FAR_BOTTOM_LEFT] - corners[FrustumCorner::NEAR_TOP_LEFT]).xyz())),
          0.0f);
    FPlanes[FrustumPlane::LEFT_PLANE].w = -Dot(FPlanes[FrustumPlane::LEFT_PLANE], corners[FrustumCorner::NEAR_TOP_LEFT]);

    FPlanes[FrustumPlane::RIGHT_PLANE] = vec4(Normalize(Cross((corners[FrustumCorner::FAR_BOTTOM_RIGHT] - corners[FrustumCorner::NEAR_TOP_RIGHT]).xyz(),
                                                    (corners[FrustumCorner::FAR_TOP_RIGHT] - corners[FrustumCorner::NEAR_TOP_RIGHT]).xyz())),
          0.0f);
    FPlanes[FrustumPlane::RIGHT_PLANE].w = -Dot(FPlanes[FrustumPlane::RIGHT_PLANE], corners[FrustumCorner::NEAR_TOP_RIGHT]);

    FPlanes[FrustumPlane::TOP_PLANE] = vec4(Normalize(Cross((corners[FrustumCorner::NEAR_TOP_RIGHT] - corners[FrustumCorner::NEAR_TOP_LEFT]).xyz(),
                                                  (corners[FrustumCorner::FAR_TOP_LEFT] - corners[FrustumCorner::NEAR_TOP_LEFT]).xyz())),
          0.0f);
    FPlanes[FrustumPlane::TOP_PLANE].w = -Dot(FPlanes[FrustumPlane::TOP_PLANE], corners[FrustumCorner::NEAR_TOP_LEFT]);

    FPlanes[FrustumPlane::BOTTOM_PLANE] = vec4(Normalize(Cross((corners[FrustumCorner::NEAR_BOTTOM_LEFT] - corners[FrustumCorner::NEAR_BOTTOM_RIGHT]).xyz(),
                                                     (corners[FrustumCorner::FAR_BOTTOM_RIGHT] - corners[FrustumCorner::NEAR_BOTTOM_RIGHT]).xyz())),
          0.0f);
    FPlanes[FrustumPlane::BOTTOM_PLANE].w = -Dot(FPlanes[FrustumPlane::BOTTOM_PLANE], corners[FrustumCorner::NEAR_BOTTOM_RIGHT]);
}

MemoryView<const vec4> Frustum::GetPlanes() const
{
    return MemoryView(FPlanes.data(), (u32)FPlanes.size());
}

} // namespace ECSEngine
