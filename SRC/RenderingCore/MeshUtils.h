#pragma once
#include "Mesh.h"
namespace ECSEngine
{
namespace Rendering
{
struct VertexLayoutHash;
namespace MeshHelpers
{

Mesh* CreateIMesh(const VertexLayoutHash& parLayoutHash);
} // namespace MeshHelpers
} // namespace Rendering
} // namespace ECSEngine