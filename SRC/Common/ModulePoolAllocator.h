#pragma once

namespace ECSEngine
{
template<typename T>
struct ModuleData
{
    T* AsPtr() { return reinterpret_cast<T*>(&FData); }
    const T* AsPtr() const { return reinterpret_cast<T*>(&FData); }

private:
    alignas(T) u8 FData[sizeof(T)];
};

template<typename T, int ChunkSize>
struct ModuleDataChunk
{
    T* AsPtr(u32 idx)
    {
        AssertRelease(idx < ChunkSize);
        return FChunk[idx].AsPtr();
    }
    const T* AsPtr(u32 idx) const
    {
        AssertRelease(idx < ChunkSize);
        return FChunk[idx].AsPtr();
    }

private:
    alignas(T) ModuleData<T> FChunk[ChunkSize];
};

template<class T, int ChunkSize, bool IsFixed>
class ModulePoolAllocator
{
    using pointer_type = T*;

public:
    ModulePoolAllocator();
    ~ModulePoolAllocator();
    void AllocateAtIndex(u32 parIndex);
    void DeallocateAtIndex(u32 parIndex);

    T* GetAtIndex(u32 parIndex);

private:
    void AllocateNewChunk();

private:
    u32 FSize;
    std::set<u32> FAllocatedIndexes;
    std::vector<ModuleDataChunk<T, ChunkSize>*> FChunks;
};

template<class T, int ChunkSize, bool IsFixed>
void ECSEngine::ModulePoolAllocator<T, ChunkSize, IsFixed>::AllocateNewChunk()
{
    AssertRelease(!IsFixed || FSize == 0);
    FChunks.push_back(new ModuleDataChunk<T, ChunkSize>());
    FSize += ChunkSize;
}

template<class T, int ChunkSize, bool IsFixed>
T* ModulePoolAllocator<T, ChunkSize, IsFixed>::GetAtIndex(u32 parIndex)
{
    AssertRelease(parIndex < FSize);
    const u32 chunkIndex = parIndex / ChunkSize;
    const u32 indexInChunk = parIndex % ChunkSize;
    return FChunks[chunkIndex]->AsPtr(indexInChunk);
}

template<class T, int ChunkSize, bool IsFixed>
ModulePoolAllocator<T, ChunkSize, IsFixed>::~ModulePoolAllocator()
{
    AlwaysCheckedAssert(FAllocatedIndexes.empty());
    for (auto it = FAllocatedIndexes.begin(); it != FAllocatedIndexes.end(); ++it)
    {
        const u32 chunkIndex = *it / ChunkSize;
        const u32 indexInChunk = *it % ChunkSize;
        FChunks[chunkIndex]->AsPtr(indexInChunk)->~T();
        delete FChunks[chunkIndex];
    }
}

template<class T, int ChunkSize, bool IsFixed>
void ModulePoolAllocator<T, ChunkSize, IsFixed>::DeallocateAtIndex(u32 parIndex)
{
    AssertRelease(parIndex < FSize);
    AssertRelease(FAllocatedIndexes.find(parIndex) != FAllocatedIndexes.end());
    const u32 chunkIndex = parIndex / ChunkSize;
    const u32 indexInChunk = parIndex % ChunkSize;
    FChunks[chunkIndex]->AsPtr(indexInChunk)->~T();
#ifdef PERFORM_SECURITY_CHECKS
    // memset(&FChunk[parIndex], 0xCD, sizeof(T));
#endif // PERFORM_SECURITY_CHECKS

    FAllocatedIndexes.erase(parIndex);
}

template<class T, int ChunkSize, bool IsFixed>
void ModulePoolAllocator<T, ChunkSize, IsFixed>::AllocateAtIndex(u32 parIndex)
{
    if (!IsFixed && parIndex >= FSize)
        AllocateNewChunk();

    AssertRelease(!IsFixed || parIndex < FSize);
    AssertRelease(FAllocatedIndexes.find(parIndex) == FAllocatedIndexes.end());

    const u32 chunkIndex = parIndex / ChunkSize;
    const u32 indexInChunk = parIndex % ChunkSize;

    pointer_type ptr = FChunks[chunkIndex]->AsPtr(indexInChunk);
    new (ptr) T;
    FAllocatedIndexes.insert(parIndex);
}

template<class T, int ChunkSize, bool IsFixed>
ModulePoolAllocator<T, ChunkSize, IsFixed>::ModulePoolAllocator()
    : FSize(0)
    , FAllocatedIndexes()
{
    AllocateNewChunk();
#ifdef PERFORM_SECURITY_CHECKS
    // memset(FChunk, 0xCD, sizeof(ModuleData<T>) * ChunkSize);
#endif // PERFORM_SECURITY_CHECKS
}
} // namespace ECSEngine
