#pragma once

namespace ECSEngine
{
class StackAllocator
{
public:
    StackAllocator(u32 parCapacity);
    ~StackAllocator();

    void* Allocate(const u32 parSize);
    void Free(const u32 parSize);

    const u32 GetAllocatedSize() const { return FAllocated; }
    const u32 CanAllocated(const u32 parSize) const { return FAllocated + parSize < FCapacity; }

private:
    uc8* FData;

    u32 FAllocated;
    u32 FCapacity;
};
} // namespace ECSEngine
