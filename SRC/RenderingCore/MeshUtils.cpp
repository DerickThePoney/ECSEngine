#include "stdafx.h"

#include "MeshUtils.h"

#include "Common/MeshStreamingData.h"
#include "VertexLayout.h"

namespace ECSEngine
{
namespace Rendering
{
namespace MeshHelpers
{

Mesh* CreateIMesh(const VertexLayoutHash& parLayoutHash)
{
    return new Mesh();
}

} // namespace MeshHelpers
} // namespace Rendering
} // namespace ECSEngine
