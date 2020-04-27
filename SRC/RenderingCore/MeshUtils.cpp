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

IMesh* CreateIMesh(const VertexLayoutHash& parLayoutHash)
{

    if (parLayoutHash.hash == VertexPosition::LayoutHash.hash)
    {
        return CreateIMesh(VertexPosition());
    }
    if (parLayoutHash.hash == VertexPositionColorN<1>::LayoutHash.hash)
    {
        return CreateIMesh(VertexPositionColorN<1>());
    }
    if (parLayoutHash.hash == VertexPositionColorN<2>::LayoutHash.hash)
    {
        return CreateIMesh(VertexPositionColorN<2>());
    }
    if (parLayoutHash.hash == VertexPositionColorN<3>::LayoutHash.hash)
    {
        return CreateIMesh(VertexPositionColorN<3>());
    }
    if (parLayoutHash.hash == VertexPositionColorN<4>::LayoutHash.hash)
    {
        return CreateIMesh(VertexPositionColorN<4>());
    }
    if (parLayoutHash.hash == VertexPositionUVNNormal<1>::LayoutHash.hash)
    {
        return CreateIMesh(VertexPositionUVNNormal<1>());
    }
    if (parLayoutHash.hash == VertexPositionUVNNormal<2>::LayoutHash.hash)
    {
        return CreateIMesh(VertexPositionUVNNormal<2>());
    }
    if (parLayoutHash.hash == VertexPositionUVNNormal<3>::LayoutHash.hash)
    {
        return CreateIMesh(VertexPositionUVNNormal<3>());
    }
    if (parLayoutHash.hash == VertexPositionUVNNormal<4>::LayoutHash.hash)
    {
        return CreateIMesh(VertexPositionUVNNormal<4>());
    }
    if (parLayoutHash.hash == VertexPositionUVNNormal<5>::LayoutHash.hash)
    {
        return CreateIMesh(VertexPositionUVNNormal<5>());
    }
    if (parLayoutHash.hash == VertexPositionUVNNormal<6>::LayoutHash.hash)
    {
        return CreateIMesh(VertexPositionUVNNormal<6>());
    }
    if (parLayoutHash.hash == VertexPositionUVNNormal<7>::LayoutHash.hash)
    {
        return CreateIMesh(VertexPositionUVNNormal<7>());
    }
    if (parLayoutHash.hash == VertexPositionUVNNormal<8>::LayoutHash.hash)
    {
        return CreateIMesh(VertexPositionUVNNormal<8>());
    }

    AssertNotReachedMsg("La hash retournée n'est pas bonne... elle ne correspond pas une hash pour un layout connu");
    return nullptr;
}

} // namespace MeshHelpers
} // namespace Rendering
} // namespace ECSEngine