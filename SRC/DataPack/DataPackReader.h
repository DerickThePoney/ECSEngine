#pragma once

#include "Common/Assertions.h"
#include "Common/Macros.h"
#include "Common/Types.h"
#include "DataPackDataStructures.h"
#include "DataPackFile.h"

#include <fstream>
#include <map>
#include <vector>

namespace ECSEngine
{
namespace DataPack
{

template<>
class DataPackFile<Access::READ>
{
public:
    ~DataPackFile() { delete[] FDataPackBuffer; }

    bool ReadDataPack(const std::string& parFilename)
    {
        std::ifstream ifstr(parFilename, std::ios::binary);
        if (!ifstr.good())
        {
            AssertNotReached();
            return false;
        }

        // read header
        ifstr.read((c8*)&FHeader, sizeof(DataPackHeader));

        // read offsets
        forrange(i, 0, FHeader.NbFiles)
        {
            FileOffset offset = 0;
            ifstr.read((c8*)&offset, sizeof(FileOffset));
            FFileOffsets.push_back(offset);
        }

        ifstr.read((c8*)&FDataPackSize, sizeof(FileOffset));

        FDataPackBuffer = new c8[FDataPackSize];
        ifstr.read(FDataPackBuffer, FDataPackSize);

        // read file names and make the correspondance
        forrange(i, 0, FHeader.NbFiles)
        {
            FileOffset currentOffset = FFileOffsets[i];
            FileRecordHeader* fileRecord = reinterpret_cast<FileRecordHeader*>(FDataPackBuffer + currentOffset);

            currentOffset += sizeof(FileRecordHeader);
            std::string fileName(FDataPackBuffer + currentOffset, fileRecord->FileNameByteSize);
            AssertRelease(FFilenameToFileOffsetMap.find(fileName) == FFilenameToFileOffsetMap.end());
            FFilenameToFileOffsetMap.insert_or_assign(fileName, (u32)i);
        }

        return true;
    }

    u32 FileExists_ReturnFileSize(const std::string& parFilename) const
    {
        auto it = FFilenameToFileOffsetMap.find(parFilename);
        if (it == FFilenameToFileOffsetMap.end())
            return -1;

        AssertRelease(it->second < (u32)FFileOffsets.size());
        FileOffset currentOffset = FFileOffsets[it->second];
        FileRecordHeader* fileRecord = reinterpret_cast<FileRecordHeader*>(FDataPackBuffer + currentOffset);
        currentOffset += sizeof(FileRecordHeader);
#ifdef ENABLE_SECURITY_CHECKS
        std::string fileName(FDataPackBuffer + currentOffset, fileRecord->FileNameByteSize);
        AssertRelease(fileName == parFilename);
#endif
        AssertRelease(fileRecord->FileNameByteSize > 0);

        return fileRecord->FileByteSize;
    }

    u32 CopyFileBuffer_AssumesSufficientCapacity(const std::string& parFilename, c8* outBuffer) const
    {
        auto it = FFilenameToFileOffsetMap.find(parFilename);
        if (it == FFilenameToFileOffsetMap.end())
            return -1;

        AssertRelease(it->second < (u32)FFileOffsets.size());
        FileOffset currentOffset = FFileOffsets[it->second];
        FileRecordHeader* fileRecord = reinterpret_cast<FileRecordHeader*>(FDataPackBuffer + currentOffset);
        currentOffset += sizeof(FileRecordHeader);

#ifdef ENABLE_SECURITY_CHECKS
        std::string fileName(FDataPackBuffer + currentOffset, fileRecord->FileNameByteSize);
        AssertRelease(fileName == parFilename);
#endif
        AssertRelease(fileRecord->FileNameByteSize > 0);

        memcpy(outBuffer, FDataPackBuffer + currentOffset + fileRecord->FileNameByteSize, fileRecord->FileByteSize);
        return fileRecord->FileByteSize;
    }

    const std::map<std::string, u32>& FilenamesToFileOffsetMap() const { return FFilenameToFileOffsetMap; }

    u32 NumberOfResources() const { return FFileOffsets.size(); }

private:
    DataPackHeader FHeader;
    std::vector<FileOffset> FFileOffsets;
    std::map<std::string, u32> FFilenameToFileOffsetMap;

    c8* FDataPackBuffer = nullptr;

    FileOffset FDataPackSize = 0;
};
} // namespace DataPack
} // namespace ECSEngine