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
class ObjectPoolAllocator
{
    using pointer_type = T*;

public:
    ObjectPoolAllocator();
    ~ObjectPoolAllocator();
    void AllocateAtIndex(u32 parIndex);
    void DeallocateAtIndex(u32 parIndex);

    T* GetAtIndex(u32 parIndex);

private:
    void AllocateNewChunk();

private:
    u32 FSize;
    std::set<u32> FAllocatedIndexes;
    std::vector<ModuleDataChunk<T, ChunkSize>> FChunks;
};

template<class T, int ChunkSize, bool IsFixed>
void ECSEngine::ObjectPoolAllocator<T, ChunkSize, IsFixed>::AllocateNewChunk()
{
    AssertRelease(!IsFixed || FSize == 0);
    FChunks.push_back(ModuleDataChunk<T, ChunkSize>());
    FSize += ChunkSize;
}

template<class T, int ChunkSize, bool IsFixed>
T* ObjectPoolAllocator<T, ChunkSize, IsFixed>::GetAtIndex(u32 parIndex)
{
    AssertRelease(parIndex < FSize);
    const u32 chunkIndex = parIndex / ChunkSize;
    const u32 indexInChunk = parIndex % ChunkSize;
    return FChunks[chunkIndex].AsPtr(indexInChunk);
}

template<class T, int ChunkSize, bool IsFixed>
ObjectPoolAllocator<T, ChunkSize, IsFixed>::~ObjectPoolAllocator()
{
    AlwaysCheckedAssert(FAllocatedIndexes.empty());
    for (auto it = FAllocatedIndexes.begin(); it != FAllocatedIndexes.end(); ++it)
    {
        const u32 chunkIndex = *it / ChunkSize;
        const u32 indexInChunk = *it % ChunkSize;
        FChunks[chunkIndex].AsPtr(indexInChunk)->~T();
    }
}

template<class T, int ChunkSize, bool IsFixed>
void ObjectPoolAllocator<T, ChunkSize, IsFixed>::DeallocateAtIndex(u32 parIndex)
{
    AssertRelease(parIndex < FSize);
    AssertRelease(FAllocatedIndexes.find(parIndex) != FAllocatedIndexes.end());
    const u32 chunkIndex = parIndex / ChunkSize;
    const u32 indexInChunk = parIndex % ChunkSize;
    FChunks[chunkIndex].AsPtr(indexInChunk)->~T();
#ifdef PERFORM_SECURITY_CHECKS
    // memset(&FChunk[parIndex], 0xCD, sizeof(T));
#endif // PERFORM_SECURITY_CHECKS

    FAllocatedIndexes.erase(parIndex);
}

template<class T, int ChunkSize, bool IsFixed>
void ObjectPoolAllocator<T, ChunkSize, IsFixed>::AllocateAtIndex(u32 parIndex)
{
    if (!IsFixed && parIndex >= FSize)
        AllocateNewChunk();

    AssertRelease(!IsFixed || parIndex < FSize);
    AssertRelease(FAllocatedIndexes.find(parIndex) == FAllocatedIndexes.end());

    const u32 chunkIndex = parIndex / ChunkSize;
    const u32 indexInChunk = parIndex % ChunkSize;

    pointer_type ptr = FChunks[chunkIndex].AsPtr(indexInChunk);
    new (ptr) T;
    FAllocatedIndexes.insert(parIndex);
}

template<class T, int ChunkSize, bool IsFixed>
ObjectPoolAllocator<T, ChunkSize, IsFixed>::ObjectPoolAllocator()
    : FSize(0)
    , FAllocatedIndexes()
{
    AllocateNewChunk();
#ifdef PERFORM_SECURITY_CHECKS
    // memset(FChunk, 0xCD, sizeof(ModuleData<T>) * ChunkSize);
#endif // PERFORM_SECURITY_CHECKS
}

#define STATIC_POOL_DECLARE(TYPE, SIZE, ISFIXED) static ECSEngine::PoolAllocator<TYPE, SIZE, ISFIXED> FPool;

#define POOL_NEW_DECLARE void* operator new(size_t size);
#define POOL_DELETE_DECLARE void operator delete(void*);
#define POOL_ARRAY_ALLOCATIONS_DELETE                                                                                                                                              \
    void* operator new[](size_t size) = delete;                                                                                                                                    \
    void operator delete[](void*);

#define DECLARE_POOL_ALLOCATED(TYPE, SIZE)                                                                                                                                         \
    STATIC_POOL_DECLARE(TYPE, SIZE, true);                                                                                                                                         \
    POOL_NEW_DECLARE;                                                                                                                                                              \
    POOL_DELETE_DECLARE;                                                                                                                                                           \
    POOL_ARRAY_ALLOCATIONS_DELETE;

#define STATIC_POOL_IMPLEMENT(TYPE, SIZE, ISFIXED) ECSEngine::PoolAllocator<TYPE, SIZE, ISFIXED> TYPE::FPool = ECSEngine::PoolAllocator<TYPE, SIZE, ISFIXED>();

#define POOL_NEW_IMPLEMENT                                                                                                                                                         \
    (TYPE) void* operator new(size_t size) {}
#define POOL_DELETE_IMPLEMENT                                                                                                                                                      \
    (TYPE) void operator delete(void*) {}

} // namespace ECSEngine
