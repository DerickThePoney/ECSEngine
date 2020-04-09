#pragma once
namespace ECSEngine
{
template<typename T>
struct PtrType
{
    T* AsPtr() { return reinterpret_cast<T*>(&FData); }
    const T* AsPtr() const { return reinterpret_cast<T*>(&FData); }

private:
    alignas(T) u8 FData[sizeof(T)];
};

template<typename T, u32 ChunkSize>
struct Chunk
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
    alignas(T) PtrType<T> FChunk[ChunkSize];
};

// struct FreeBlock
//{
//    FreeBlock()
//        : first(0)
//        , second(0)
//    {
//    }
//
//    FreeBlock(const u32 parFirst, const u32 parSecond)
//        : first(parFirst)
//        , second(parFirst)
//    {
//        AssertRelease(second >= first);
//    }
//
//    FreeBlock(const FreeBlock& other)
//    {
//        first = other.first;
//        second = other.second;
//    }
//
//    FreeBlock(FreeBlock&& other)
//    {
//        first = other.first;
//        second = other.second;
//    }
//
//    void operator=(const FreeBlock& other)
//    {
//        first = other.first;
//        second = other.second;
//    }
//
//    void operator=(FreeBlock&& other)
//    {
//        first = other.first;
//        second = other.second;
//    }
//
//    bool operator==(const FreeBlock& other) { return first == other.first && second == other.second; }
//
//    u32 size() { return second - first + 1; }
//
//    u32 first;
//    u32 second;
//};

// TODO Free chunks should be saved in pairs start end, and merged on release.
template<class T, u32 ChunkSize>
class PoolAllocator
{
    static_assert(ChunkSize > 0, "Impossible de pooler avec des chunks de taille 0");
    using pooled_type = T;
    using ChunkIndexPair = std::pair<u32, u32>;
    using FreeBlock = std::pair<u32, u32>;

public:
    PoolAllocator();
    ~PoolAllocator();

    void* Allocate(const u32 parNumber);
    void Free(void* parPtr, const u32 parNumber);

private:
    void* FindContiguousAllocationSpot(const u32 parNumber);

    ChunkIndexPair GetChunkAndIndexNumberForPtr(void* parPtr);

    void GrowOneChunk();

private:
    std::map<u32, std::list<FreeBlock>> FFreeBlocks;
    std::vector<Chunk<pooled_type, ChunkSize>*> FChunks;
};

template<class T, u32 ChunkSize>
PoolAllocator<T, ChunkSize>::PoolAllocator()
{
}

template<class T, u32 ChunkSize>
PoolAllocator<T, ChunkSize>::~PoolAllocator()
{
    forrange(i, 0, FChunks.size()) delete FChunks[i];
}

template<class T, u32 ChunkSize>
void* PoolAllocator<T, ChunkSize>::Allocate(const u32 parNumber)
{
    AlwaysCheckedAssert(parNumber < ChunkSize);
    AlwaysCheckedAssert(parNumber > 0);

    return FindContiguousAllocationSpot(parNumber);
}

template<class T, u32 ChunkSize>
void PoolAllocator<T, ChunkSize>::Free(void* parPtr, const u32 parNumber)
{
    ChunkIndexPair foundPosition = GetChunkAndIndexNumberForPtr(parPtr);
    AssertRelease((foundPosition.first != -1) && (foundPosition.second != -1));

    AssertRelease((ChunkSize - foundPosition.second) >= parNumber);

    FreeBlock newFreeBlock(foundPosition.second, foundPosition.second + parNumber - 1);
    AssertRelease(newFreeBlock.first < ChunkSize && newFreeBlock.second < ChunkSize);

    AssertRelease(FFreeBlocks.find(foundPosition.first) != FFreeBlocks.end());
    std::list<FreeBlock>& chunkFreeBlocks = FFreeBlocks[foundPosition.first];
    auto itFound = chunkFreeBlocks.end();
    bool needToAdd = false;
    bool found = false;
    for (auto it = chunkFreeBlocks.begin(); it != chunkFreeBlocks.end(); ++it)
    {
        FreeBlock& currentFreeBlock = *it;
        AssertRelease(newFreeBlock.first < currentFreeBlock.first || newFreeBlock.first > currentFreeBlock.second);
        AssertRelease(newFreeBlock.second < currentFreeBlock.first || newFreeBlock.second > currentFreeBlock.second);

        if (newFreeBlock.second < currentFreeBlock.first)
        {
            if ((newFreeBlock.second + 1) == currentFreeBlock.first)
            {
                currentFreeBlock.first = newFreeBlock.first;
                found = true;
                break;
            }
            else
            {
                itFound = it;
                needToAdd = true;
                found = true;
                break;
            }
        }
        else if (newFreeBlock.first > currentFreeBlock.second)
        {
            if ((newFreeBlock.first) == (currentFreeBlock.second + 1))
            {
                currentFreeBlock.second = newFreeBlock.second;
                found = true;
                break;
            }
        }
        else
        {
            AssertNotReached();
        }
    }

    if (needToAdd || !found)
        chunkFreeBlocks.insert(itFound, newFreeBlock);
}

template<class T, u32 ChunkSize>
std::pair<u32, u32> PoolAllocator<T, ChunkSize>::GetChunkAndIndexNumberForPtr(void* parPtr)
{
    forrange(i, 0, FChunks.size())
    {
        const std::ptrdiff_t diff = reinterpret_cast<T*>(parPtr) - FChunks[i]->AsPtr(0);
        if (diff >= 0 && diff < ChunkSize)
            return { (u32)i, (u32)diff };
    }

    return { -1, -1 };
}

template<class T, u32 ChunkSize>
void* PoolAllocator<T, ChunkSize>::FindContiguousAllocationSpot(const u32 parNumber)
{
    AlwaysCheckedAssert(parNumber < ChunkSize);
    AlwaysCheckedAssert(parNumber > 0);

    if (parNumber > ChunkSize)
        return nullptr;

    bool foundSpot = false;
    std::list<FreeBlock>::iterator itFound;
    u32 chunkId = -1;
    foreachitem(chunkFreeBlocks, FFreeBlocks)
    {
        if (chunkFreeBlocks.second.size() < parNumber)
            continue;

        auto itInList = chunkFreeBlocks.second.begin();
        itFound = chunkFreeBlocks.second.end();
        u32 sizeFound = 0;
        foundSpot = false;
        chunkId = chunkFreeBlocks.first;

        for (; itInList != chunkFreeBlocks.second.end(); ++itInList)
        {
            FreeBlock& currentFreeBlock = *itInList;
            if ((currentFreeBlock.second - currentFreeBlock.first + 1) >= parNumber)
            {
                foundSpot = true;
                itFound = itInList;
                break;
            }
        }

        if (foundSpot)
            break;
    }

    if (!foundSpot)
    {
        chunkId = (u32)FChunks.size();
        GrowOneChunk();
        itFound = FFreeBlocks[chunkId].begin();
    }

    void* result = FChunks[chunkId]->AsPtr(itFound->first);

    std::list<FreeBlock>& freeBlocksToRemove = FFreeBlocks[chunkId];

    if ((itFound->second - itFound->first + 1) == parNumber)
    {
        freeBlocksToRemove.erase(itFound);
    }
    else
    {
        itFound->first = itFound->first + parNumber;
        AssertRelease(itFound->second >= itFound->first);
    }

    return result;
}

template<class T, u32 ChunkSize>
void PoolAllocator<T, ChunkSize>::GrowOneChunk()
{
    const u32 chunkId = (u32)FChunks.size();
    FChunks.push_back(new Chunk<T, ChunkSize>());

    std::list<FreeBlock> newFreeblocks;
    FreeBlock newFreeBlock = { 0, ChunkSize - 1 };
    newFreeblocks.push_back(newFreeBlock);
    AssertRelease((newFreeblocks.begin()->second - newFreeblocks.begin()->first + 1) == ChunkSize);
    FFreeBlocks.insert_or_assign(chunkId, newFreeblocks);
}

#define DECLARE_POOL_ALLOCATED_CUSTOM_CHUNK_SIZE(TYPE, CHUNK_SIZE)                                                                                                                 \
public:                                                                                                                                                                            \
    static PoolAllocator<TYPE, CHUNK_SIZE> sPool##TYPE;                                                                                                                            \
    static void* operator new(std::size_t count) { return TYPE::sPool##TYPE.Allocate((u32)(count / sizeof(TYPE))); }                                                               \
    static void operator delete(void* ptr, std::size_t count) { TYPE::sPool##TYPE.Free(ptr, (u32)(count / sizeof(TYPE))); }                                                        \
    static void* operator new[](std::size_t count) { return TYPE::sPool##TYPE.Allocate((u32)(count / sizeof(TYPE))); }                                                             \
    static void operator delete[](void* ptr, std::size_t count) { TYPE::sPool##TYPE.Free(ptr, (u32)(count / sizeof(TYPE))); }                                                      \
                                                                                                                                                                                   \
private:

#define IMPLEMENT_POOL_ALLOCATED_CUSTOM_CHUNK_SIZE(TYPE, CHUNK_SIZE) PoolAllocator<TYPE, CHUNK_SIZE> TYPE::sPool##TYPE = PoolAllocator<TYPE, CHUNK_SIZE>();

#define DECLARE_POOL_ALLOCATED(TYPE) DECLARE_POOL_ALLOCATED_CUSTOM_CHUNK_SIZE(TYPE, 1024)
#define IMPLEMENT_POOL_ALLOCATED(TYPE) IMPLEMENT_POOL_ALLOCATED_CUSTOM_CHUNK_SIZE(TYPE, 1024)

} // namespace ECSEngine
