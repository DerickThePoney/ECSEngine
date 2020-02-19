#pragma once
namespace ECSEngine
{
class ResourceHandle;
class IResourceLoader
{
public:
    virtual const std::string& GetPattern() const = 0;
    virtual bool UseRawFile() const = 0;
    virtual u32 GetLoadedResourceSize(const c8* parRawBuffer, u32 parRawSize) = 0;
    virtual bool LoadResource(c8* parRawBuffer, u32 parRawSize, std::shared_ptr<ResourceHandle> parHandle) = 0;
};

class DefaultResourceLoader : public IResourceLoader
{
public:
    virtual const std::string& GetPattern() const override
    {
        static std::string pattern = "*";
        return pattern;
    }

    virtual bool UseRawFile() const override;

    virtual u32 GetLoadedResourceSize(const c8* parRawBuffer, u32 parRawSize) override;

    virtual bool LoadResource(c8* parRawBuffer, u32 parRawSize, std::shared_ptr<ResourceHandle> parHandle) override;
};
} // namespace ECSEngine
