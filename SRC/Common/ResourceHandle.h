#pragma once
#include "Resource.h"

namespace ECSEngine
{
class ResourceCache;

struct ResourceBuffer : public std::streambuf
{
    ResourceBuffer(char* begin, u32 size) { this->setg(begin, begin, begin + size); }
};

class ResourceHandle
{
public:
    ResourceHandle(Resource& parResource, c8* parBuffer, u32 parSize, ResourceCache* parResourceCache);
    virtual ~ResourceHandle();

    const Resource& GetResource() const { return FResource; }
    const u32 Size() const { return FSize; }
    const c8* Buffer() const { return FBuffer; }
    ResourceBuffer GetResourceBuffer() { return ResourceBuffer(FBuffer, FSize); }

    c8* WritableBuffer() { return FBuffer; }

protected:
    Resource FResource;
    c8* FBuffer;
    u32 FSize;
    ResourceCache* FResourceCache;
};
} // namespace ECSEngine