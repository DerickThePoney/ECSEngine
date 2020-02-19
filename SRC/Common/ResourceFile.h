#pragma once

namespace ECSEngine
{
class Resource;
class IResourceFile
{
public:
    virtual bool Open() = 0;
    virtual u32 GetRawResourceSize(const Resource& r) = 0;
    virtual u32 GetRawResource(const Resource& r, c8* buffer) = 0;
    virtual u32 GetNumResources() const = 0;
    virtual std::string GetResourceName(i32 num) const = 0;
    virtual ~IResourceFile() {}
};
} // namespace ECSEngine