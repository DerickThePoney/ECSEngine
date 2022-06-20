#include "stdafx.h"

#include "SaveLoadBuffer.h"

#include "MathHelpers.h"

namespace ECSEngine
{
namespace SavingSystem
{
IMPLEMENT_POOL_ALLOCATED(Buffer);
void Buffer::WriteData(u32 parId, u32 parSize, u8* parData)
{
    GrowToAccomodateAtLeast(parSize + 2 * sizeof(u32));

    memcpy(&FData[FWrittenBytes], &parId, sizeof(u32));
    FWrittenBytes += sizeof(u32);
    memcpy(&FData[FWrittenBytes], &parSize, sizeof(u32));
    FWrittenBytes += sizeof(u32);
    memcpy(&FData[FWrittenBytes], parData, parSize);
    FWrittenBytes += parSize;
}

void Buffer::WriteRawData(u32 parSize, u8* parData)
{
    GrowToAccomodateAtLeast(parSize + sizeof(u32));
    memcpy(&FData[FWrittenBytes], &parSize, sizeof(u32));
    FWrittenBytes += sizeof(u32);
    memcpy(&FData[FWrittenBytes], parData, parSize);
    FWrittenBytes += parSize;
}

void Buffer::WriteGuards(u32 parId)
{
    GrowToAccomodateAtLeast(2 * sizeof(u32));

    u32 size = 0;
    memcpy(&FData[FWrittenBytes], &parId, sizeof(u32));
    FWrittenBytes += sizeof(u32);

    memcpy(&FData[FWrittenBytes], &size, sizeof(u32));
    FWrittenBytes += sizeof(u32);
}

void Buffer::WriteSize(u32 parSize)
{
    GrowToAccomodateAtLeast(sizeof(u32));
    memcpy(&FData[FWrittenBytes], &parSize, sizeof(u32));
    FWrittenBytes += sizeof(u32);
}

void Buffer::WriteIdAndSize(u32 parId, u32 parSize)
{
    GrowToAccomodateAtLeast(2 * sizeof(u32));

    memcpy(&FData[FWrittenBytes], &parId, sizeof(u32));
    FWrittenBytes += sizeof(u32);

    memcpy(&FData[FWrittenBytes], &parSize, sizeof(u32));
    FWrittenBytes += sizeof(u32);
}

u32 Buffer::ReadId()
{
    u32 id;
    memcpy(&id, &FData[FReadBytes], sizeof(u32));
    IncrementReadData(sizeof(u32));
    return id;
}

u32 Buffer::ReadSize()
{
    u32 size;
    memcpy(&size, &FData[FReadBytes], sizeof(u32));
    IncrementReadData(sizeof(u32));
    return size;
}

void Buffer::ReadData(u32 parSize, u8* parData)
{
    memcpy(parData, &FData[FReadBytes], parSize);
    IncrementReadData(parSize);
}

void Buffer::GrowToAccomodateAtLeast(u32 parSize)
{
    u32 nextPow2 = MathHelpers::NextPowerOfTwo(FWrittenBytes + parSize);
    if (FData.size() < nextPow2)
        FData.resize(nextPow2, 0);
}

void Buffer::IncrementReadData(u32 parValue)
{
    FReadBytes += parValue;
    AssertRelease(FReadBytes <= FWrittenBytes);
}

} // namespace SavingSystem
} // namespace ECSEngine
