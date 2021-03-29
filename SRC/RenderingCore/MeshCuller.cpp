#include "stdafx.h"

#include "MeshCuller.h"

#include "Common/IntersectionRoutines.h"
#include "ECSGameplay_Common/ApparenceModule.h"
#include "Mesh.h"
#include "MeshManager.h"

namespace ECSEngine
{
namespace Rendering
{
namespace MeshFrustumCulling
{

bool CullApparenceModule(const ApparenceModule& parApparenceModule, const glm::mat4& parLocalToWorldMatrix, const Frustum& parFrustum)
{
    const MeshHandle& meshHandle = parApparenceModule.GetMeshHandle();
    AlwaysCheckedAssert(meshHandle.IsValid());

    return CullMesh(meshHandle, parLocalToWorldMatrix, parFrustum);
}

bool CullMesh(const MeshHandle& parMeshHandle, const glm::mat4& parLocalToWorldMatrix, const Frustum& parFrustum)
{
    const Mesh* mesh = MeshManager::Instance().GetMesh(parMeshHandle);
    AssertRelease(mesh != nullptr);

    glm::vec4 sphere = mesh->BoundingCircle();

    const float radius = sphere.w;
    sphere.w = 1.0f;

    sphere = parLocalToWorldMatrix * sphere;

    // TODO extract scale for radius
    sphere.w = radius;

    return Intersection::FrustumSphereIntersect(parFrustum, sphere);
}

} // namespace MeshFrustumCulling
} // namespace Rendering
} // namespace ECSEngine
