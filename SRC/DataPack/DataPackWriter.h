#pragma once

#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
#include "DataPackDataStructures.h"
#include "DataPackFile.h"

namespace ECSEngine
{
namespace DataPack
{

template<>
class DataPackFile<Access::WRITE>
{
public:
    DataPackFile()
        : FFilesStream(std::ios::binary)
    {
    }

    void PushFile(const Resource* parResource)
    {
        AssertRelease(parResource != nullptr);
        std::shared_ptr<ResourceHandle> handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(parResource);

        if (handle == nullptr)
        {
            AssertNotReached();
            return;
        }

        FileRecordHeader fileRecordHeader;
        fileRecordHeader.FileByteSize = handle->Size();
        fileRecordHeader.FileNameByteSize = parResource->FName.size();

        FFilesOffsets.push_back(FCurrentOffset);
        FCurrentOffset += sizeof(FileRecordHeader) + fileRecordHeader.FileNameByteSize + fileRecordHeader.FileByteSize;

        FHeader.NbFiles = FHeader.NbFiles + 1;
        AlwaysCheckedAssert(FHeader.NbFiles == FFilesOffsets.size());

        FFilesStream.write((c8*)&fileRecordHeader, sizeof(FileRecordHeader));
        FFilesStream.write(parResource->FName.c_str(), fileRecordHeader.FileNameByteSize);
        FFilesStream.write(handle->Buffer(), fileRecordHeader.FileByteSize);
    }

    void Finalize(const std::string& parFilename)
    {
        // FFilesStream << std::ends;

        std::ofstream ofstr(parFilename, std::ios::binary);
        if (!ofstr.good())
        {
            AssertNotReached();
            return;
        }

        ofstr.write((c8*)&FHeader, sizeof(DataPackHeader));

        forrange(i, 0, FFilesOffsets.size())
        {
            const FileOffset idx = FFilesOffsets[i];
            ofstr.write((c8*)&idx, sizeof(FileOffset));
        }
        ofstr.write((c8*)&FCurrentOffset, sizeof(FileOffset));
        AssertRelease(FFilesStream.str().size() == FCurrentOffset);
        ofstr.write(FFilesStream.str().c_str(), FCurrentOffset);
    }

private:
    std::ostringstream FFilesStream;
    DataPackHeader FHeader;
    std::vector<FileOffset> FFilesOffsets;
    FileOffset FCurrentOffset = 0;
};
} // namespace DataPack
} // namespace ECSEngine
