#pragma once

namespace ECSEngine
{
class ApparenceModule;
class Frustum;
namespace Rendering
{
class MeshHandle;
namespace MeshFrustumCulling
{
bool CullApparenceModule(const ApparenceModule& parApparenceModule, const glm::mat4& parLocalToWorldMatrix, const Frustum& parFrustum);
bool CullMesh(const MeshHandle& parMeshHandle, const glm::mat4& parLocalToWorldMatrix, const Frustum& parFrustum);
} // namespace MeshFrustumCulling
} // namespace Rendering
} // namespace ECSEngine
