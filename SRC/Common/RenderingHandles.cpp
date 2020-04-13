#include "stdafx.h"

#include "RenderingHandles.h"

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
    return FMeshId != HandlesId::InvalidMeshIdHandle;
}

MaterialInstanceHandle::MaterialInstanceHandle(u32 parMaterialId /*= HandlesId::InvalideMaterialHandle*/)
    : FMaterialInstanceId(parMaterialId)
{
}

bool MaterialInstanceHandle::IsValid() const
{
    return FMaterialInstanceId != HandlesId::InvalidMaterialInstanceHandle;
}

} // namespace Rendering
} // namespace ECSEngine
