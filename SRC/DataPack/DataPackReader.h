#pragma once

#include "DataPackDataStructures.h"
#include "DataPackFile.h"

namespace ECSEngine
{
namespace DataPack
{

template<>
class DataPackFile<Access::READ>
{
public:
    ~DataPackFile() { delete[] FDataPackBuffer; }

    void ReadDataPack(const std::string& parFilename)
    {
        std::ifstream ifstr(parFilename, std::ios::binary);
        if (!ifstr.good())
        {
            AssertNotReached();
            return;
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
    }

private:
    DataPackHeader FHeader;
    std::vector<FileOffset> FFileOffsets;
    std::map<std::string, u32> FFilenameToFileOffsetMap;

    c8* FDataPackBuffer = nullptr;

    FileOffset FDataPackSize = 0;
};
} // namespace DataPack
} // namespace ECSEngine