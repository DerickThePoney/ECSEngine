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

    template<typename T>
    void operator&(std::vector<T>& parValue)
    {
        u32 id = typeid(std::vector<T>).hash_code();
        u32 size = parValue.size();
        WriteIdAndSize(id, size);
        foreachitem(value, parValue) this->operator&(value);
    }

    template<typename T, typename U>
    void operator&(std::pair<T, U>& parValue)
    {
        u32 id = typeid(std::vector<T>).hash_code();
        WriteIdAndSize(id, 0);
        this->operator&(parValue.first);
        this->operator&(parValue.second);
    }

    Buffer& GetBuffer() { return FDataBuffer; }

private:
    void WriteIdAndSize(u32 id, u32 size);

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

    template<typename T>
    void operator&(std::vector<T>& parValue)
    {
        u32 expectedid = typeid(std::vector<T>).hash_code();

        u32 id, size;
        ReadIdAndSize(id, size);
        AlwaysCheckedAssert(id == expectedid);
        parValue.resize(size);
        foreachitem(value, parValue) this->operator&(value);
    }

    template<typename T, typename U>
    void operator&(std::pair<T, U>& parValue)
    {
        u32 expectedid = typeid(std::pair<T, U>).hash_code();

        u32 id, size;
        ReadIdAndSize(id, size);
        AlwaysCheckedAssert(id == expectedid);
        AlwaysCheckedAssert(size == 0);

        this->operator&(parValue.first);
        this->operator&(parValue.second);
    }

    Buffer& GetBuffer() { return FDataBuffer; }
    void SetBuffer(const Buffer& parBuffer) { FDataBuffer = parBuffer; }

private:
    void ReadIdAndSize(u32& id, u32& size);

private:
    Buffer FDataBuffer;
};
} // namespace SavingSystem

void GenericLoad(SavingSystem::ReadChunk& parChunk, u8* parData, u32 expectedId, u32 expectedSize);
} // namespace ECSEngine