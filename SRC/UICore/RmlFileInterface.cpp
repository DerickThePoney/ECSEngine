#include "stdafx.h"

#include "RmlFileInterface.h"

#include "Common/ResourceCache.h"

namespace ECSEngine
{
namespace UI
{
IMPLEMENT_POOL_ALLOCATED_INTERNAL(RmlFileInterface, InternalFileHandle);

Rml::FileHandle RmlFileInterface::Open(const Rml::String& path)
{
    Resource file(path);
    const u32 index = IsFileAlreadyOpened(file);
    AlwaysCheckedAssert(index == -1);
    if (index != -1)
        return (Rml::FileHandle)(index + 1);

    auto handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&file);
    if (handle == nullptr)
        return (Rml::FileHandle)0;

    const u32 newFileHandle = GetNextIndexForNewActiveFile();
    FActiveFileHandles[newFileHandle]->FFileHandle = handle;
    return (Rml::FileHandle)(newFileHandle + 1);
}

void RmlFileInterface::Close(Rml::FileHandle file)
{
    const u32 index = ConvertToIndex(file);

    FActiveFileHandles[index].reset(nullptr);
}

size_t RmlFileInterface::Read(void* buffer, size_t size, Rml::FileHandle file)
{
    const u32 index = ConvertToIndex(file);

    const u32 dataStart = FActiveFileHandles[index]->FReadPtr;
    const u32 fileEnd = FActiveFileHandles[index]->FFileHandle->Size();
    AlwaysCheckedAssert(dataStart <= fileEnd);
    if (dataStart >= fileEnd)
        return 0;

    const u32 dataToCopy = std::min((fileEnd - dataStart), (u32)size);
    memcpy(buffer, FActiveFileHandles[index]->FFileHandle->Buffer() + dataStart, dataToCopy);

    FActiveFileHandles[index]->FReadPtr += dataToCopy;
    return dataToCopy;
}

bool RmlFileInterface::Seek(Rml::FileHandle file, long offset, int origin)
{
    const u32 index = ConvertToIndex(file);
    auto fh = FActiveFileHandles[index].get();
    const u32 fileSize = fh->FFileHandle->Size();

    const u32 currentReadPtr = FActiveFileHandles[index]->FReadPtr;

    switch (origin)
    {
    case SEEK_SET:
        if (offset > fileSize)
            return false;
        if (offset < 0)
            return false;
        fh->FReadPtr = (u32)offset;
    case SEEK_END:
        if (offset > fileSize)
            return false;
        if (offset > 0)
            return false;
        fh->FReadPtr = fileSize - (u32)offset;
    case SEEK_CUR:
        long newReadPtr = offset + currentReadPtr;
        if (newReadPtr < 0)
            return false;
        if (newReadPtr > fileSize)
            return false;
        fh->FReadPtr = (u32)newReadPtr;
    }
    return true;
}

size_t RmlFileInterface::Tell(Rml::FileHandle file)
{
    const u32 index = ConvertToIndex(file);
    auto fh = FActiveFileHandles[index].get();
    return fh->FReadPtr;
}

size_t RmlFileInterface::Length(Rml::FileHandle file)
{
    const u32 index = ConvertToIndex(file);
    auto fh = FActiveFileHandles[index].get();
    return fh->FFileHandle->Size();
}

u32 RmlFileInterface::IsFileAlreadyOpened(const Resource& parName)
{
    u32 i = 0;
    foreachitemconst(fh, FActiveFileHandles)
    {
        if (fh == nullptr)
            continue;
        if (fh->FFileHandle == nullptr)
            continue;
        if (fh->FFileHandle->GetResource().FName == parName.FName)
        {
            return i;
        }

        ++i;
    }

    return -1;
}

u32 RmlFileInterface::GetNextIndexForNewActiveFile()
{
    forrange(i, 0, FActiveFileHandles.size())
    {
        if (FActiveFileHandles[i] == nullptr)
        {
            FActiveFileHandles[i] = std::make_unique<InternalFileHandle>();
            return i;
        }
    }

    const u32 index = FActiveFileHandles.size();
    FActiveFileHandles.push_back(std::make_unique<InternalFileHandle>());
    return index;
}

u32 RmlFileInterface::ConvertToIndex(Rml::FileHandle file)
{
    AssertRelease(file != 0);
    const u32 index = ((u32)file) - 1;
    AssertRelease(index < FActiveFileHandles.size());
    AssertRelease(FActiveFileHandles[index] != nullptr);
    return index;
}

} // namespace UI
} // namespace ECSEngine