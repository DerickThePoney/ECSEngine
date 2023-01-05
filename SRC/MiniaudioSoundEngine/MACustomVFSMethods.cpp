#include "stdafx.h"

#include "MACustomVFSMethods.h"

#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
#include "Common/Singleton.h"
#include "SoundCore/SoundResourceFileSystem.h"

namespace ECSEngine
{

class MAAudioFileSystemInterface : public Singleton<MAAudioFileSystemInterface>
{
public:
    ma_result ma_vfs_open(ma_vfs* pVFS, const char* pFilePath, ma_uint32 openMode, ma_vfs_file* pFile);
    ma_result ma_vfs_close(ma_vfs* pVFS, ma_vfs_file file);
    ma_result ma_vfs_read(ma_vfs* pVFS, ma_vfs_file file, void* pDst, size_t sizeInBytes, size_t* pBytesRead);
    ma_result ma_vfs_seek(ma_vfs* pVFS, ma_vfs_file file, ma_int64 offset, ma_seek_origin origin);
    ma_result ma_vfs_tell(ma_vfs* pVFS, ma_vfs_file file, ma_int64* pCursor);
    ma_result ma_vfs_info(ma_vfs* pVFS, ma_vfs_file file, ma_file_info* pInfo);

private:
    u64 IsFileAlreadyOpened(const Resource& parName);
    u64 GetNextIndexForNewActiveFile();
    u64 ConvertToIndex(ma_vfs_file file);

private:
    struct InternalFileHandle
    {
    public:
        std::shared_ptr<ResourceHandle> FFileHandle = nullptr;
        u32 FReadPtr = 0;
    };

    std::vector<std::unique_ptr<InternalFileHandle>> FActiveFileHandles;
};

ma_result MAAudioFileSystemInterface::ma_vfs_open(ma_vfs* pVFS, const char* pFilePath, ma_uint32 openMode, ma_vfs_file* pFile)
{
    ECSEngine::Resource file(pFilePath);

    printf("Opening %s\n", pFilePath);

    auto handle = SoundResourceCache::Instance().FCache->GetResourceHandle(&file);
    if (handle == nullptr)
    {
        *pFile = (void*)(0);
        return MA_INVALID_FILE;
    }

    const u64 index = IsFileAlreadyOpened(file);
    if (index != -1)
    {
        *pFile = (void*)(index + 1);
        return MA_SUCCESS;
    }

    const u64 newFileHandle = GetNextIndexForNewActiveFile();
    FActiveFileHandles[newFileHandle]->FFileHandle = handle;
    *pFile = (void*)(newFileHandle + 1);
    return MA_SUCCESS;
}

ma_result MAAudioFileSystemInterface::ma_vfs_close(ma_vfs* pVFS, ma_vfs_file file)
{
    const u64 index = ConvertToIndex(file);

    if (index == -1)
        return MA_INVALID_FILE;

    printf("Closing %s\n", FActiveFileHandles[index]->FFileHandle->GetResource().FName.c_str());
    FActiveFileHandles[index].reset(nullptr);
    return MA_SUCCESS;
}

ma_result MAAudioFileSystemInterface::ma_vfs_read(ma_vfs* pVFS, ma_vfs_file file, void* pDst, size_t sizeInBytes, size_t* pBytesRead)
{
    const u64 index = ConvertToIndex(file);
    printf("Reading %zd bytes in %s\n", sizeInBytes, FActiveFileHandles[index]->FFileHandle->GetResource().FName.c_str());

    const u32 dataStart = FActiveFileHandles[index]->FReadPtr;
    const u32 fileEnd = FActiveFileHandles[index]->FFileHandle->Size();
    if (dataStart >= fileEnd)
        return MA_AT_END;

    const u32 dataToCopy = ((fileEnd - dataStart) < (u32)sizeInBytes) ? (fileEnd - dataStart) : (u32)sizeInBytes;
    memcpy(pDst, FActiveFileHandles[index]->FFileHandle->Buffer() + dataStart, dataToCopy);

    FActiveFileHandles[index]->FReadPtr += dataToCopy;
    *pBytesRead = dataToCopy;
    return MA_SUCCESS;
}

ma_result MAAudioFileSystemInterface::ma_vfs_seek(ma_vfs* pVFS, ma_vfs_file file, ma_int64 offset, ma_seek_origin origin)
{
    const u64 index = ConvertToIndex(file);
    printf("Seeking %s\n", FActiveFileHandles[index]->FFileHandle->GetResource().FName.c_str());
    auto fh = FActiveFileHandles[index].get();
    const u32 fileSize = fh->FFileHandle->Size();

    const u32 currentReadPtr = FActiveFileHandles[index]->FReadPtr;

    switch (origin)
    {
    case ma_seek_origin_start:
        if (offset > fileSize)
            return MA_ERROR;
        if (offset < 0)
            return MA_ERROR;
        fh->FReadPtr = (u32)offset;
        break;
    case ma_seek_origin_end:
        if (offset > fileSize)
            return MA_ERROR;
        if (offset > 0)
            return MA_ERROR;
        fh->FReadPtr = fileSize - (u32)offset;
        break;
    case ma_seek_origin_current:
        long newReadPtr = offset + currentReadPtr;
        if (newReadPtr < 0)
            return MA_ERROR;
        if (newReadPtr > fileSize)
            return MA_ERROR;
        fh->FReadPtr = (u32)newReadPtr;
        break;
    }
    return MA_SUCCESS;
}

ma_result MAAudioFileSystemInterface::ma_vfs_tell(ma_vfs* pVFS, ma_vfs_file file, ma_int64* pCursor)
{
    const u64 index = ConvertToIndex(file);
    printf("Telling %s\n", FActiveFileHandles[index]->FFileHandle->GetResource().FName.c_str());
    auto fh = FActiveFileHandles[index].get();
    *pCursor = fh->FReadPtr;
    return MA_SUCCESS;
}

ma_result MAAudioFileSystemInterface::ma_vfs_info(ma_vfs* pVFS, ma_vfs_file file, ma_file_info* pInfo)
{
    const u64 index = ConvertToIndex(file);
    printf("Infoing %s\n", FActiveFileHandles[index]->FFileHandle->GetResource().FName.c_str());
    auto fh = FActiveFileHandles[index].get();
    pInfo->sizeInBytes = fh->FFileHandle->Size();
    return MA_SUCCESS;
}

u64 MAAudioFileSystemInterface::IsFileAlreadyOpened(const ECSEngine::Resource& parName)
{
    u64 i = 0;
    for (const std::unique_ptr<InternalFileHandle>& fh : FActiveFileHandles)
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

u64 MAAudioFileSystemInterface::GetNextIndexForNewActiveFile()
{
    for (u64 i = 0; i < FActiveFileHandles.size(); ++i)
    {
        if (FActiveFileHandles[i] == nullptr)
        {
            FActiveFileHandles[i] = std::make_unique<InternalFileHandle>();
            return i;
        }
    }

    const u64 index = FActiveFileHandles.size();
    FActiveFileHandles.push_back(std::make_unique<InternalFileHandle>());
    return index;
}

u64 MAAudioFileSystemInterface::ConvertToIndex(ma_vfs_file file)
{
    const u64 index = ((u64)file) - 1;
    return index;
}

namespace MACustomVFSMethods
{

void CreateVFS()
{
    MAAudioFileSystemInterface::CreateIFP();
}

void CloseVFS()
{
    MAAudioFileSystemInterface::Destroy();
}

ma_result ma_vfs_open(ma_vfs* pVFS, const char* pFilePath, ma_uint32 openMode, ma_vfs_file* pFile)
{
    return MAAudioFileSystemInterface::Instance().ma_vfs_open(pVFS, pFilePath, openMode, pFile);
}

ma_result ma_vfs_close(ma_vfs* pVFS, ma_vfs_file file)
{
    return MAAudioFileSystemInterface::Instance().ma_vfs_close(pVFS, file);
}

ma_result ma_vfs_read(ma_vfs* pVFS, ma_vfs_file file, void* pDst, size_t sizeInBytes, size_t* pBytesRead)
{
    return MAAudioFileSystemInterface::Instance().ma_vfs_read(pVFS, file, pDst, sizeInBytes, pBytesRead);
}

ma_result ma_vfs_seek(ma_vfs* pVFS, ma_vfs_file file, ma_int64 offset, ma_seek_origin origin)
{
    return MAAudioFileSystemInterface::Instance().ma_vfs_seek(pVFS, file, offset, origin);
}

ma_result ma_vfs_tell(ma_vfs* pVFS, ma_vfs_file file, ma_int64* pCursor)
{
    return MAAudioFileSystemInterface::Instance().ma_vfs_tell(pVFS, file, pCursor);
}

ma_result ma_vfs_info(ma_vfs* pVFS, ma_vfs_file file, ma_file_info* pInfo)
{
    return MAAudioFileSystemInterface::Instance().ma_vfs_info(pVFS, file, pInfo);
}

} // namespace MACustomVFSMethods
} // namespace ECSEngine
