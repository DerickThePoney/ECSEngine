#pragma once

namespace ECSEngine
{
class Frustum;
namespace Rendering
{
class MeshHandle;
namespace MeshFrustumCulling
{
bool CullMesh(const MeshHandle& parMeshHandle, const mat4& parLocalToWorldMatrix, const Frustum& parFrustum);
} // namespace MeshFrustumCulling
} // namespace Rendering
} // namespace ECSEngine
