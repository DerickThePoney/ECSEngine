#include "stdafx.h"

#include "ResourceCache.h"

#include "../ECSCore/AdjustableDebugParameters.h"
#include "Resource.h"
#include "ResourceFile.h"
#include "ResourceHandle.h"
#include "ResourceLoader.h"
#include "StringUtilities.h"

namespace ECSEngine
{

ResourceCache::ResourceCache(const u32 parSizeInMb, IResourceFile* parFileSystem)
    : FCacheSize(parSizeInMb << 20)
    , FAllocated(0)
    , FFileSystem(parFileSystem)
{
    AssertRelease(FFileSystem != nullptr);
}

ResourceCache::~ResourceCache()
{
    while (!FLRU.empty())
    {
        FreeOneResource();
    }

    if (FFileSystem != nullptr)
    {
        delete FFileSystem;
        FFileSystem = nullptr;
    }
}

bool ResourceCache::Initialize()
{
    AssertRelease(FFileSystem != nullptr);
    bool retValue = false;

    if (FFileSystem->Open())
    {
        RegisterLoader(std::unique_ptr<IResourceLoader>(new DefaultResourceLoader()));
        retValue = true;
    }

    return retValue;
}

void ResourceCache::RegisterLoader(std::unique_ptr<IResourceLoader>&& parLoader)
{
#ifdef PERFORM_SECURITY_CHECKS
    foreachitemconst(loader, FResourceLoaders)
    {
        AlwaysCheckedAssertMsg(parLoader != loader, "Trying to register the same loader twice!!");
    }
#endif

    FResourceLoaders.push_front(std::move(parLoader));
}

void ResourceCache::ReOpenFileSystem()
{
    AssertRelease(FFileSystem != nullptr);
    AssertRelease(FFileSystem->Open());
}

std::shared_ptr<ResourceHandle> ResourceCache::GetResourceHandle(const Resource* parResource)
{
    AssertRelease(FFileSystem != nullptr);
    std::shared_ptr<ResourceHandle> handle(Find(parResource));
    if (handle == nullptr)
        handle = Load(parResource);
    else
        Update(handle);
    return handle;
}

bool ResourceCache::FileExists(Resource* parResource)
{
    return FFileSystem->FileExists(parResource->FName);
}

const std::string& ResourceCache::GetBasePath() const
{
    AssertRelease(FFileSystem != nullptr);
    return FFileSystem->GetBasePathName();
}

const char* ResourceCache::GetFileSystemInfo() const
{
    return FFileSystem->FileSystemInfo();
}

std::shared_ptr<ECSEngine::ResourceHandle> ResourceCache::Find(const Resource* parResource)
{
    auto itFind = FResources.find(parResource->FName);
    if (itFind != FResources.end())
        return itFind->second;

    return nullptr;
}

void ResourceCache::Update(std::shared_ptr<ResourceHandle> parHandle)
{
    AlwaysCheckedAssert(parHandle != nullptr);
    for (auto lruHandle = FLRU.begin(); lruHandle != FLRU.end(); ++lruHandle)
    {
        if (*lruHandle == parHandle)
        {
            FLRU.erase(lruHandle);
            break;
        }
    }

    FLRU.push_front(parHandle);
}

std::shared_ptr<ResourceHandle> ResourceCache::Load(const Resource* parResource)
{
    AssertRelease(FFileSystem != nullptr);
    IResourceLoader* loader = nullptr;
    std::shared_ptr<ResourceHandle> handle;

    for (auto it = FResourceLoaders.begin(); it != FResourceLoaders.end(); ++it)
    {
        IResourceLoader* testLoader = it->get();
        AssertRelease(testLoader != nullptr);
        if (StringUtilities::WildcardMatch(testLoader->GetPattern().c_str(), parResource->FName.c_str()))
        {
            loader = testLoader;
            break;
        }
    }

    if (loader == nullptr)
    {
        AlwaysCheckedAssertMsg(loader != nullptr, "The loader has not been found for the specified resource!");
        return handle;
    }

    const bool useRawFile = loader->UseRawFile();
    u32 rawSize = FFileSystem->GetRawResourceSize(*parResource);
    c8* rawBuffer = useRawFile ? Allocate(rawSize) : new c8[rawSize];

    AlwaysCheckedAssert(rawBuffer != nullptr);
    if (rawBuffer == nullptr)
    {
        return nullptr;
    }

    FFileSystem->GetRawResource(*parResource, rawBuffer);
    c8* buffer = nullptr;
    u32 size = 0;

    if (useRawFile)
    {
        buffer = rawBuffer;
        handle = std::shared_ptr<ResourceHandle>(new ResourceHandle(*parResource, buffer, rawSize, this));
    }
    else
    {
        size = loader->GetLoadedResourceSize(rawBuffer, rawSize);
        buffer = Allocate(size);
        AlwaysCheckedAssert(rawBuffer != nullptr || buffer != nullptr);
        if (rawBuffer == nullptr || buffer == nullptr)
        {
            return nullptr;
        }

        handle = std::shared_ptr<ResourceHandle>(new ResourceHandle(*parResource, buffer, size, this));

        bool success = loader->LoadResource(rawBuffer, rawSize, handle);
        AlwaysCheckedAssertMsg(success, "Impossible to load specified resource!");
        AssertRelease(rawBuffer != nullptr);
        delete[] rawBuffer;
        rawBuffer = nullptr;

        if (!success)
            return nullptr;
    }

    if (handle != nullptr)
    {
        FLRU.push_front(handle);
        AssertRelease(FResources.find(parResource->FName) == FResources.end());
        FResources[parResource->FName] = handle;
    }
    AlwaysCheckedAssert(handle != nullptr);
    return handle;
}

void ResourceCache::Free(std::shared_ptr<ResourceHandle> parResourceHandle)
{
    AlwaysCheckedAssert(parResourceHandle != nullptr);
    for (auto lruHandle = FLRU.begin(); lruHandle != FLRU.end(); ++lruHandle)
    {
        if (*lruHandle == parResourceHandle)
        {
            FLRU.erase(lruHandle);
            break;
        }
    }

    FResources.erase(parResourceHandle->GetResource().FName);
}

bool ResourceCache::MakeRoom(u32 parSize)
{
    if (parSize + FAllocated > FCacheSize)
        return false;

    while (parSize > FCacheSize - FAllocated)
    {
        if (FLRU.empty())
            return false;
        FreeOneResource();
    }
    return true;
}

c8* ResourceCache::Allocate(u32 parSize)
{
    if (!MakeRoom(parSize))
        return nullptr;

    c8* memory = new c8[parSize];
    AlwaysCheckedAssert(memory != nullptr);
    if (memory != nullptr)
    {
        FAllocated += parSize;
    }

    return memory;
}

void ResourceCache::FreeOneResource()
{
    auto gonner = FLRU.end();
    gonner--;

    std::shared_ptr<ResourceHandle> handle = *gonner;

    FLRU.pop_back();
    FResources.erase(handle->GetResource().FName);
}

void ResourceCache::MemoryHasBeenFreed(u32 parSize)
{
    AlwaysCheckedAssert(FAllocated >= parSize);
    FAllocated -= parSize;
}

} // namespace ECSEngine
