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

template<class T, int InitSize, bool IsFixed>
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
    u32 FSize;
    std::set<u32> FAllocatedIndexes;
    alignas(T) ModuleData<T> FChunk[InitSize];
};

template<class T, int InitSize, bool IsFixed>
T* ObjectPoolAllocator<T, InitSize, IsFixed>::GetAtIndex(u32 parIndex)
{
    AssertRelease(parIndex < FSize);
    return FChunk[parIndex].AsPtr();
}

template<class T, int InitSize, bool IsFixed>
ObjectPoolAllocator<T, InitSize, IsFixed>::~ObjectPoolAllocator()
{
    AlwaysCheckedAssert(FAllocatedIndexes.empty());
    for (auto it = FAllocatedIndexes.begin(); it != FAllocatedIndexes.end(); ++it)
    {
        FChunk[*it].AsPtr()->~T();
    }
}

template<class T, int InitSize, bool IsFixed>
void ObjectPoolAllocator<T, InitSize, IsFixed>::DeallocateAtIndex(u32 parIndex)
{
    AssertRelease(parIndex < FSize);
    AssertRelease(FAllocatedIndexes.find(parIndex) != FAllocatedIndexes.end());
    FChunk[parIndex].AsPtr()->~T();
#ifdef PERFORM_SECURITY_CHECKS
    memset(&FChunk[parIndex], 0xCD, sizeof(T));
#endif // PERFORM_SECURITY_CHECKS

    FAllocatedIndexes.erase(parIndex);
}

template<class T, int InitSize, bool IsFixed>
void ObjectPoolAllocator<T, InitSize, IsFixed>::AllocateAtIndex(u32 parIndex)
{
    AssertRelease(IsFixed && parIndex < FSize);
    AssertRelease(FAllocatedIndexes.find(parIndex) == FAllocatedIndexes.end());
    pointer_type ptr = FChunk[parIndex].AsPtr();
    new (ptr) T;
    FAllocatedIndexes.insert(parIndex);
}

template<class T, int InitSize, bool IsFixed>
ObjectPoolAllocator<T, InitSize, IsFixed>::ObjectPoolAllocator()
    : FSize(InitSize)
    , FAllocatedIndexes()
{
#ifdef PERFORM_SECURITY_CHECKS
    memset(FChunk, 0xCD, sizeof(ModuleData<T>) * InitSize);
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
