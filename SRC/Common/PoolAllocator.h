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

template<class T, u32 ChunkSize>
class PoolAllocator
{
    static_assert(ChunkSize > 0, "Impossible de pooler avec des chunks de taille 0");
    using pooled_type = T;
    using ChunkIndexPair = std::pair<u32, u32>;

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
    std::map<u32, std::list<u32>> FFreeBlocks;
    std::vector<Chunk<pooled_type, ChunkSize>*> FChunks;
};

template<class T, u32 ChunkSize>
PoolAllocator<T, ChunkSize>::PoolAllocator()
{
}

template<class T, u32 ChunkSize>
PoolAllocator<T, ChunkSize>::~PoolAllocator()
{
    //    AlwaysCheckedAssert(FFreeBlocks.size() == 0);
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

    std::list<u32> itemsToAddBack;
    forrange(i, 0, (std::size_t)parNumber) { itemsToAddBack.push_back(foundPosition.second + (u32)i); }

    std::list<u32>& chunkFreeBlocks = FFreeBlocks[foundPosition.first];
    auto itFound = chunkFreeBlocks.end();
    for (auto it = chunkFreeBlocks.begin(); it != chunkFreeBlocks.end(); ++it)
    {
        if (*it > *itemsToAddBack.rbegin())
        {
            itFound = it;
            break;
        }
    }

    chunkFreeBlocks.insert(itFound, itemsToAddBack.begin(), itemsToAddBack.end());
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
    std::list<u32>::iterator itFound;
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
        while (itInList != chunkFreeBlocks.second.end() && sizeFound != parNumber && !foundSpot)
        {
            itFound = itInList;
            sizeFound++;

            if (sizeFound == parNumber)
            {
                foundSpot = true;
                break;
            }

            while (itInList != chunkFreeBlocks.second.end() && sizeFound != parNumber && !foundSpot)
            {
                itInList++;

                if ((*itInList - *itFound) > sizeFound)
                {
                    sizeFound = 0;
                    break;
                }
                ++sizeFound;
                if (sizeFound == parNumber)
                {
                    foundSpot = true;
                    break;
                }
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

    void* result = FChunks[chunkId]->AsPtr(*itFound);

    std::list<u32>& freeBlocksToRemove = FFreeBlocks[chunkId];
    for (u32 i = 0; i < parNumber; ++i)
    {
        itFound = freeBlocksToRemove.erase(itFound);
    }

    return result;
}

template<class T, u32 ChunkSize>
void PoolAllocator<T, ChunkSize>::GrowOneChunk()
{
    const u32 chunkId = (u32)FChunks.size();
    FChunks.push_back(new Chunk<T, ChunkSize>());

    std::list<u32> newFreeblocks;
    forrange(i, 0, (std::size_t)ChunkSize) { newFreeblocks.push_back((u32)i); }
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
