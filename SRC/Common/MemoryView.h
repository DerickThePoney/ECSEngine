#pragma once

namespace ECSEngine
{
template<typename T>
class MemoryView
{
public:
    class MemoryViewIterator
    {
    public:
        MemoryViewIterator(MemoryView<T>* parMemoryView, u32 parIdx = 0)
            : FMemoryView(parMemoryView)
            , FIdx(parIdx)
        {
        }

        bool operator==(const MemoryViewIterator& other) { return FIdx == other.FIdx; }
        bool operator!=(const MemoryViewIterator& other) { return FIdx != other.FIdx; }

        void operator++(int) { FIdx++; }
        void operator++() { ++FIdx; }

        T& operator*() { return *(FMemoryView->FData + FIdx); }
        T& operator->() = delete;

    private:
        MemoryView<T>* FMemoryView;
        u32 FIdx;
    };

    class MemoryViewConstIterator
    {
    public:
        MemoryViewConstIterator(MemoryView<T>* parMemoryView, u32 parIdx = 0)
            : FMemoryView(parMemoryView)
            , FIdx(parIdx)
        {
        }

        bool operator==(const MemoryViewConstIterator& other) { return FIdx == other.FIdx; }
        bool operator!=(const MemoryViewConstIterator& other) { return FIdx != other.FIdx; }

        void operator++(int) { FIdx++; }
        void operator++() { ++FIdx; }

        const T& operator*() { return *(FMemoryView->FData + FIdx); }
        const T& operator->() = delete;

    private:
        MemoryView<T>* FMemoryView;
        u32 FIdx;
    };

    class MemoryViewReverseIterator
    {
    public:
        MemoryViewReverseIterator(MemoryView<T>* parMemoryView, u32 parIdx)
            : FMemoryView(parMemoryView)
            , FIdx(parIdx)
        {
        }

        bool operator==(const MemoryViewReverseIterator& other) { return FIdx == other.FIdx; }
        bool operator!=(const MemoryViewReverseIterator& other) { return FIdx != other.FIdx; }

        void operator++(int) { FIdx--; }
        void operator++() { --FIdx; }

        T& operator*() { return *(FMemoryView->FData + FIdx); }
        T& operator->() = delete;

    private:
        MemoryView<T>* FMemoryView;
        u32 FIdx;
    };

    class MemoryViewReverseConstIterator
    {
    public:
        MemoryViewReverseConstIterator(MemoryView<T>* parMemoryView, u32 parIdx)
            : FMemoryView(parMemoryView)
            , FIdx(parIdx)
        {
        }

        bool operator==(const MemoryViewReverseConstIterator& other) { return FIdx == other.FIdx; }
        bool operator!=(const MemoryViewReverseConstIterator& other) { return FIdx != other.FIdx; }

        void operator++(int) { FIdx--; }
        void operator++() { --FIdx; }

        T& operator*() { return *(FMemoryView->FData + FIdx); }
        T& operator->() = delete;

    private:
        MemoryView<T>* FMemoryView;
        u32 FIdx;
    };

    friend class MemoryViewIterator;
    friend class MemoryViewConstIterator;
    friend class MemoryViewReverseIterator;
    friend class MemoryViewReverseConstIterator;

    using iterator = MemoryViewIterator;
    using const_iterator = MemoryViewConstIterator;
    using reverse_iterator = MemoryViewReverseIterator;
    using const_reverse_iterator = MemoryViewReverseConstIterator;

public:
    MemoryView(T* parData, const u32 parSize)
        : FData(parData)
        , FSize(parSize)
    {
    }

    MemoryView(T* parData, const std::size_t parSize)
        : FData(parData)
        , FSize((u32)parSize)
    {
        AlwaysCheckedAssert(std::numeric_limits<u32>::max() >= parSize);
    }

    iterator begin() { return iterator(this, 0); }
    const_iterator cbegin() { return const_iterator(this, 0); }
    reverse_iterator rbegin() { return reverse_iterator(this, FSize - 1); }
    const_reverse_iterator crbegin() { return const_reverse_iterator(this, FSize - 1); }

    iterator end() { return iterator(this, FSize); }
    const_iterator cend() { return const_iterator(this, FSize); }
    reverse_iterator rend() { return reverse_iterator(this, (u32)-1); }
    const_reverse_iterator crend() { return const_reverse_iterator(this, (u32)-1); }

    u32 size() const { return FSize; }

    const T& operator[](const u32 parIndex) const
    {
        AssertRelease(parIndex < FSize);
        return *(FData + parIndex);
    }

private:
    T* FData;
    u32 FSize;
};
} // namespace ECSEngine