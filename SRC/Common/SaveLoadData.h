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
    void operator&(const T& parValue)
    {
        SaveLoad<SaveChunk, T, true>(*this, (T&)parValue); // ouip. Deso.
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
        u32 id = typeid(std::pair<T, U>).hash_code();
        WriteIdAndSize(id, 0);
        this->operator&(parValue.first);
        this->operator&(parValue.second);
    }

    template<typename T, typename U>
    void operator&(std::map<T, U>& parValue)
    {
        u32 id = typeid(std::map<T, U>).hash_code();
        u32 size = parValue.size();
        WriteIdAndSize(id, size);

        foreachitem(value, parValue)
        {
            this->operator&(value.first);
            this->operator&(value.second);
        }
    }

    template<typename T>
    void operator&(std::list<T>& parValue)
    {
        u32 id = typeid(std::list<T>).hash_code();
        u32 size = parValue.size();
        WriteIdAndSize(id, size);

        foreachitem(value, parValue) this->operator&(value);
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

    template<typename T, typename U>
    void operator&(std::map<T, U>& parValue)
    {
        u32 expectedid = typeid(std::map<T, U>).hash_code();

        u32 id, size;
        ReadIdAndSize(id, size);
        AlwaysCheckedAssert(id == expectedid);

        forrange(i, 0, size)
        {
            T key;
            this->operator&(key);
            U val;
            this->operator&(val);
            parValue.insert_or_assign(key, val);
        }
    }

    template<typename T>
    void operator&(std::list<T>& parValue)
    {
        u32 expectedid = typeid(std::list<T>).hash_code();

        u32 id, size;
        ReadIdAndSize(id, size);
        AlwaysCheckedAssert(id == expectedid);

        parValue.resize(size);
        foreachitem(value, parValue) this->operator&(value);
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