#include "stdafx.h"

#include "Texture.h"

#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
#include "TextureDescriptor.h"

namespace ECSEngine
{
namespace Rendering
{
//----------------------------------------------------------------
//          Texture
//----------------------------------------------------------------
IMPLEMENT_POOL_ALLOCATED(Texture);

Texture::Texture(const TextureDescriptor* parTextureDescriptor, const std::string& parTextureName)
    : RefCountedObject()
    , FTextureDescriptor(parTextureDescriptor)
    , FTextureName(parTextureName)
{
}

Texture::~Texture()
{
    FTextureDescriptor = nullptr;
    if (Valid())
        Unload();
}

void Texture::SetTextureData_IKnowWhatImDoing(bgfx::TextureHandle parHandle, bgfx::TextureInfo parInfo)
{
    FHandle = parHandle;
    FInfo = parInfo;
}

void Texture::Load()
{
    AssertRelease(FTextureDescriptor != nullptr);
    const std::string& textureFile = FTextureDescriptor->TextureFile();
    std::size_t found = textureFile.find_last_of('\\');
    AssertRelease(found != textureFile.npos);
    Resource r(textureFile.substr(0, found + 1) + FTextureName + ".ktx");
    std::shared_ptr<ResourceHandle> handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&r);
    AssertRelease(handle != nullptr);

    FHandle = bgfx::createTexture(bgfx::copy(handle->Buffer(), handle->Size()), FTextureDescriptor->Flags(), (u8)0, &FInfo);
    AssertRelease(Valid());
}

void Texture::Unload()
{
    bgfx::destroy(FHandle);
}

bool Texture::Valid() const
{
    return bgfx::isValid(FHandle);
}

} // namespace Rendering
} // namespace ECSEngine
