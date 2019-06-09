#pragma once

namespace ECSEngine
{

template<class T, int InitSize, bool IsFixed>
class ModulePoolAllocator
{
public:
    ModulePoolAllocator();
    ~ModulePoolAllocator();
    void AllocateAtIndex(u32 parIndex);
    void DeallocateAtIndex(u32 parIndex);

    T* GetAtIndex(u32 parIndex);

private:
    T* FData;
    u32 FSize;
    std::set<u32> FAllocatedIndexes;
};

template<class T, int InitSize, bool IsFixed>
T* ModulePoolAllocator<T, InitSize, IsFixed>::GetAtIndex(u32 parIndex)
{
    AssertRelease(parIndex < FSize);
    return &FData[parIndex];
}

template<class T, int InitSize, bool IsFixed>
ModulePoolAllocator<T, InitSize, IsFixed>::~ModulePoolAllocator()
{
    AlwaysCheckedAssert(FAllocatedIndexes.empty());
    for (auto it = FAllocatedIndexes.begin(); it != FAllocatedIndexes.end(); ++it)
    {
        FData[*it].~T();
    }
    free(FData);
}

template<class T, int InitSize, bool IsFixed>
void ModulePoolAllocator<T, InitSize, IsFixed>::DeallocateAtIndex(u32 parIndex)
{
    AssertRelease(parIndex < FSize);
    AssertRelease(FAllocatedIndexes.find(parIndex) != FAllocatedIndexes.end());
    FData[parIndex].~T();
#ifdef PERFORM_SECURITY_CHECKS
    memset(&FData[parIndex], 0xCD, sizeof(T));
#endif // PERFORM_SECURITY_CHECKS

    FAllocatedIndexes.erase(parIndex);
}

template<class T, int InitSize, bool IsFixed>
void ModulePoolAllocator<T, InitSize, IsFixed>::AllocateAtIndex(u32 parIndex)
{
    AssertRelease(IsFixed && parIndex < FSize);
    AssertRelease(FAllocatedIndexes.find(parIndex) == FAllocatedIndexes.end());
    ::new (&FData[parIndex]) T();
    FAllocatedIndexes.insert(parIndex);
}

template<class T, int InitSize, bool IsFixed>
ModulePoolAllocator<T, InitSize, IsFixed>::ModulePoolAllocator()
    : FSize(InitSize)
{

    FData = reinterpret_cast<T*>(malloc(FSize * sizeof(T)));
    memset(FData, 0xCD, InitSize * sizeof(T));
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
