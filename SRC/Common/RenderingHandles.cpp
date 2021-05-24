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

TextureHandle::TextureHandle(u32 parBankId /*= HandlesId::InvalidTextureBankId*/, u32 parTextureHandle /*= HandlesId::InvalidTextureHandle*/)
    : FBankId(parBankId)
    , FTextureHandleId(parTextureHandle)
{
}

bool TextureHandle::IsValid() const
{
    return FTextureHandleId != HandlesId::InvalidTextureHandle;
}

TextureName::TextureName(const std::string& parBankName, const std::string& parTextureName)
    : FBankName(parBankName)
    , FTexture(parTextureName)
{
}

TextureName::~TextureName()
{
}

} // namespace Rendering
} // namespace ECSEngine
