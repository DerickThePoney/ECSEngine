#pragma once
#include "Mesh.h"
namespace ECSEngine
{
namespace Rendering
{
struct VertexLayoutHash;
namespace MeshHelpers
{
template<typename VertexLayout>
IMesh* CreateIMesh(const VertexLayout& parLayout)
{
    return new Mesh<VertexLayout>();
}

IMesh* CreateIMesh(const VertexLayoutHash& parLayoutHash);
} // namespace MeshHelpers
} // namespace Rendering
} // namespace ECSEngine