#include "stdafx.h"

#include "Mesh.h"

namespace ECSEngine
{
namespace Rendering
{
namespace MeshHelpers
{

Mesh* CreateIMesh()
{
    return new Mesh();
}

} // namespace MeshHelpers
} // namespace Rendering
} // namespace ECSEngine
