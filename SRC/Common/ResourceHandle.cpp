#include "stdafx.h"

#include "ResourceHandle.h"

#include "ResourceCache.h"
namespace ECSEngine
{
ResourceHandle::ResourceHandle(Resource& parResource, c8* parBuffer, u32 parSize, ResourceCache* parResourceCache)
    : FResource(parResource)
    , FBuffer(parBuffer)
    , FSize(parSize)
    , FResourceCache(parResourceCache)
{
}

ResourceHandle::~ResourceHandle()
{
    if (FBuffer != nullptr)
        delete[] FBuffer;
    FBuffer = nullptr;

    FResourceCache->MemoryHasBeenFreed(FSize);
}

} // namespace ECSEngine
