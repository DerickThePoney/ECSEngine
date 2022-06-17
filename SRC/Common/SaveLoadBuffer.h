#pragma once
#include "PoolAllocator.h"

namespace ECSEngine
{
namespace SavingSystem
{
class Buffer
{
    DECLARE_POOL_ALLOCATED(Buffer);

public:
    void WriteData(u32 parId, u32 parSize, u8* parData);
    void WriteGuards(u32 parId);

    u32 WrittenBytes() const { return FWrittenBytes; }
    const u8* Data() const { return FData.data(); }

    u32 ReadId();
    u32 ReadSize();
    void ReadData(u32 parSize, u8* parData);

private:
    void GrowAtLeast(u32 parSize);
    void IncrementReadData(u32 parValue);

private:
    std::vector<u8> FData;
    u32 FWrittenBytes = 0;
    u32 FReadBytes = 0;
};
} // namespace SavingSystem
} // namespace ECSEngine