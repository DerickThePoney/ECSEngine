#ifdef DECLARING_PARAMETERS
#include "Common/RenderingHandles.h"
#include "ECSGameplay_Specific/CircularGridAccessor.h"
#include "EntityId.h"
namespace ECSEngine
{
namespace Rendering
{
class MeshHandle;
}
namespace ModuleParameters
{
#endif

DECLARE_MODULE_PARAMETER(Position, vec3)
DECLARE_MODULE_PARAMETER(Orientation, quat)
DECLARE_MODULE_PARAMETER(EulerAnglesXYZ, vec3)
DECLARE_MODULE_PARAMETER(Mesh, Rendering::MeshHandle)
DECLARE_MODULE_PARAMETER(OwnerId, EntityId)
DECLARE_MODULE_PARAMETER(GridAccessor, CircularGridAccessor)

#ifdef DECLARING_PARAMETERS
}
} // namespace ECSEngine
#endif
