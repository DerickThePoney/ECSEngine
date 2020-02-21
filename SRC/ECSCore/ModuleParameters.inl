#ifdef DECLARING_PARAMETERS
#include "Common/MeshHandle.h"
namespace ECSEngine
{
namespace Rendering
{
class MeshHandle;
}
namespace ModuleParameters
{
#endif

DECLARE_MODULE_PARAMETER(Position, glm::vec3)
DECLARE_MODULE_PARAMETER(Orientation, glm::vec3)
DECLARE_MODULE_PARAMETER(Mesh, Rendering::MeshHandle)
// DECLARE_MODULE_PARAMETER(Material, bgfx::ProgramHandle)

#ifdef DECLARING_PARAMETERS
}
} // namespace ECSEngine
#endif