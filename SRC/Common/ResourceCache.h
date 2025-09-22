#pragma once
#include "Singleton.h"

#include <list>

namespace ECSEngine
{
class IResourceLoader;
class ResourceHandle;
class IResourceFile;
class Resource;

using ResourceHandleList = std::list<std::shared_ptr<ResourceHandle>>;
using ResourceHandleMap = std::map<std::string, std::shared_ptr<ResourceHandle>>;
using ResourceLoaders = std::list<std::unique_ptr<IResourceLoader>>;
class ResourceCache
{
    friend class ResourceHandle;

public:
    ResourceCache(const u32 parSizeInMb, IResourceFile* parFileSystem);
    ~ResourceCache();

    bool Initialize();
    void RegisterLoader(std::unique_ptr<IResourceLoader>&& parLoader);

    void ReOpenFileSystem();

    std::shared_ptr<ResourceHandle> GetResourceHandle(const Resource* parResource);
    void ForceReleaseResource(std::shared_ptr<ResourceHandle>& parHandle);

    bool FileExists(Resource* parResource);

    const std::string& GetBasePath() const;

    const IResourceFile* GetFileSystem() const { return FFileSystem; }

    u32 CacheSize() const { return FCacheSize; }
    u32 Allocated() const { return FAllocated; }

    const ResourceHandleMap& AllocatedResources() const { return FResources; }
    const char* GetFileSystemInfo() const;

private:
    std::shared_ptr<ResourceHandle> Find(const Resource* parResource);
    void Update(std::shared_ptr<ResourceHandle> parHandle);

    std::shared_ptr<ResourceHandle> Load(const Resource* parResource);
    void Free(std::shared_ptr<ResourceHandle> parResourceHandle);

    bool MakeRoom(u32 parSize);
    c8* Allocate(u32 parSize);
    void FreeOneResource();
    void MemoryHasBeenFreed(u32 parSize);

private:
    ResourceHandleList FLRU;
    ResourceHandleMap FResources;
    ResourceLoaders FResourceLoaders;

    IResourceFile* FFileSystem;

    u32 FCacheSize;
    u32 FAllocated;
};

class GlobalResourceCache final : public Singleton<GlobalResourceCache>
{
public:
    GlobalResourceCache()
        : Singleton<GlobalResourceCache>()
        , FCache(nullptr)
    {
    }

    ~GlobalResourceCache()
    {
        if (FCache != nullptr)
        {
            delete FCache;
            FCache = nullptr;
        }
    }

public:
    ResourceCache* FCache;
};
} // namespace ECSEngine
