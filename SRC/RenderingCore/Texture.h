#pragma once
#include "Common/PoolAllocator.h"
#include "Common/RefCountedObject.h"

namespace ECSEngine
{
namespace Rendering
{
class TextureDescriptor;
//----------------------------------------------------------------
//          Texture
//----------------------------------------------------------------
class Texture : public RefCountedObject
{
    DECLARE_POOL_ALLOCATED(Texture);

public:
    Texture(const TextureDescriptor* parTextureDescriptor);
    ~Texture();

    void Load();
    void Unload();
    bool Valid() const;

    const bgfx::TextureHandle& Handle() const { return FHandle; }
    const bgfx::TextureInfo& Info() const { return FInfo; }

    const TextureDescriptor* Descriptor() const { return FTextureDescriptor; }

private:
    const TextureDescriptor* FTextureDescriptor;

    bgfx::TextureHandle FHandle;
    bgfx::TextureInfo FInfo;
};
} // namespace Rendering
} // namespace ECSEngine
