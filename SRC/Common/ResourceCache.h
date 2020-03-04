#pragma once
#include "Singleton.h"

namespace ECSEngine
{
class IResourceLoader;
class ResourceHandle;
class IResourceFile;
class Resource;

using ResourceHandleList = std::list<std::shared_ptr<ResourceHandle>>;
using ResourceHandleMap = std::map<std::string, std::shared_ptr<ResourceHandle>>;
using ResourceLoaders = std::list<std::shared_ptr<IResourceLoader>>;
class ResourceCache
{
    friend class ResourceHandle;

public:
    ResourceCache(const u32 parSizeInMb, IResourceFile* parFileSystem);
    ~ResourceCache();

    bool Initialize();
    void RegisterLoader(std::shared_ptr<IResourceLoader> parLoader);

    std::shared_ptr<ResourceHandle> GetResourceHandle(Resource* parResource);
    i32 Preload(std::string parPattern, void (*parProgressCallback)(i32, bool&));
    void Flush();

    const std::string& GetBasePath() const;

    const IResourceFile* GetFileSystem() const { return FFileSystem; }

private:
    std::shared_ptr<ResourceHandle> Find(Resource* parResource);
    void Update(std::shared_ptr<ResourceHandle> parHandle);

    std::shared_ptr<ResourceHandle> Load(Resource* parResource);
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
