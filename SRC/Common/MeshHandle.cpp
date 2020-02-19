#include "stdafx.h"

#include "MeshHandle.h"

namespace ECSEngine
{
namespace Rendering
{

MeshHandle::MeshHandle(u32 parMeshId /*= MeshHandleId::InvalidMeshIdHandle*/)
    : FMeshId(parMeshId)
{
}

bool MeshHandle::IsValid() const
{
    return FMeshId != MeshHandleId::InvalidMeshIdHandle;
}

} // namespace Rendering
} // namespace ECSEngine
