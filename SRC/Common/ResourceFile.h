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
    virtual void ListResourceFiles(const std::string& parWildcardPattern, std::vector<std::string>& parOutFileList) const = 0;
    virtual bool FileExists(const std::string& parFileName) const = 0;
    virtual ~IResourceFile() {}

    virtual const std::string& GetBasePathName() const = 0;
};
} // namespace ECSEngine