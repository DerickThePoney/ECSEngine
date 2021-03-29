#include "stdafx.h"

#include "MeshDescriptor.h"

namespace ECSEngine
{
namespace Rendering
{

MeshDescriptor::MeshDescriptor()
{
}

MeshDescriptor::~MeshDescriptor()
{
}

const MeshHandle MeshDescriptor::CreateMesh()
{
    AssertNotReachedMsg("Not implemented");
    return MeshHandle();
}

} // namespace Rendering
} // namespace ECSEngine
