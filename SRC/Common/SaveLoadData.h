#pragma once
#include "SaveLoadBuffer.h"

namespace ECSEngine
{
template<typename Chunk, typename T, bool isWriting>
void SaveLoad(Chunk& parChunk, T& parValue);

namespace SavingSystem
{
class SaveChunk
{
public:
    template<typename T>
    void operator&(T& parValue)
    {
        SaveLoad<SaveChunk, T, true>(*this, parValue);
    }

    Buffer& GetBuffer() { return FDataBuffer; }

private:
    Buffer FDataBuffer;
};

class ReadChunk
{
public:
    template<typename T>
    void operator&(T& parValue)
    {
        SaveLoad<ReadChunk, T, false>(*this, parValue);
    }

    Buffer& GetBuffer() { return FDataBuffer; }
    void SetBuffer(const Buffer& parBuffer) { FDataBuffer = parBuffer; }

private:
    Buffer FDataBuffer;
};
} // namespace SavingSystem
} // namespace ECSEngine