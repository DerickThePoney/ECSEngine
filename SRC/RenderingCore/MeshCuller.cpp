#include "stdafx.h"

#include "MeshCuller.h"

#include "Common/IntersectionRoutines.h"
#include "Mesh.h"
#include "MeshManager.h"

namespace ECSEngine
{
namespace Rendering
{
namespace MeshFrustumCulling
{

bool CullMesh(const MeshHandle& parMeshHandle, const mat4& parLocalToWorldMatrix, const Frustum& parFrustum)
{
    const Mesh* mesh = MeshManager::Instance().GetMesh(parMeshHandle);
    AssertRelease(mesh != nullptr);

    vec4 sphere = mesh->BoundingCircle();

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
