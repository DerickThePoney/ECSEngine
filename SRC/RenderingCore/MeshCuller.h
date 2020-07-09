#pragma once

namespace ECSEngine
{
class ApparenceModule;
class Frustum;
namespace Rendering
{
namespace MeshFrustumCulling
{
bool CullApparenceModule(const ApparenceModule& parApparenceModule, const glm::mat4& parLocalToWorldMatrix, const Frustum& parFrustum);
}
} // namespace Rendering
} // namespace ECSEngine