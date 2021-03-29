#pragma once
#include "FixedSizedArray.h"

namespace ECSEngine
{
template<typename T, u32 Size>
class RingBuffer : public FixedSizedArrayInSitu<T, Size>
{
public:
    RingBuffer()
        : FixedSizedArrayInSitu<T, Size>()
        , FWriteHead(0)
    {
    }
    RingBuffer(const T& val)
        : FixedSizedArrayInSitu<T, Size>(val)
        , FWriteHead(0)
    {
    }
    RingBuffer(const RingBuffer<T, Size>& other)
        : FixedSizedArrayInSitu<T, Size>(other)
        , FWriteHead(other.FWriteHead)
    {
    }
    RingBuffer(RingBuffer<T, Size>&& other)
        : FixedSizedArrayInSitu<T, Size>(other)
        , FWriteHead(other.FWriteHead)
    {
    }

    virtual ~RingBuffer() {}
    RingBuffer<T, Size>& operator=(const RingBuffer<T, Size>& other)
    {
        FixedSizedArrayInSitu<T, Size>::operator=(other);
        FWriteHead = other.FWriteHead;
        return *this;
    }
    RingBuffer<T, Size>& operator=(RingBuffer<T, Size>&& other)
    {
        FixedSizedArrayInSitu<T, Size>::operator=(other);
        FWriteHead = other.FWriteHead;
        return *this;
    }

    void Push(const T& val)
    {
        FixedSizedArrayInSitu<T, Size>::operator[](FWriteHead) = val;
        FWriteHead = (FWriteHead + 1) % Size;
    }

    const u32 GetWriteHeadPosition() const { return FWriteHead; }

private:
    u32 FWriteHead;
};
} // namespace ECSEngine
