#pragma once
namespace ECSEngine
{

template<typename T, u32 Size>
class FixedSizedArrayInSitu
{
    class FixedSizedArrayInSituIterator
    {
    public:
        FixedSizedArrayInSituIterator(FixedSizedArrayInSitu<T, Size>* parFixedSizedArrayInSitu, u32 parIdx = 0)
            : FFixedSizedArrayInSitu(parFixedSizedArrayInSitu)
            , FIdx(parIdx)
        {
        }

        bool operator==(const FixedSizedArrayInSituIterator& other) { return FIdx == other.FIdx; }
        bool operator!=(const FixedSizedArrayInSituIterator& other) { return FIdx != other.FIdx; }

        void operator++(int) { FIdx++; }
        void operator++() { ++FIdx; }

        T& operator*() { return FFixedSizedArrayInSitu->FData[FIdx]; }
        T& operator->() = delete;

    private:
        FixedSizedArrayInSitu<T, Size>* FFixedSizedArrayInSitu;
        u32 FIdx;
    };

    class FixedSizedArrayInSituConstIterator
    {
    public:
        FixedSizedArrayInSituConstIterator(FixedSizedArrayInSitu<T, Size>* parFixedSizedArrayInSitu, u32 parIdx = 0)
            : FFixedSizedArrayInSitu(parFixedSizedArrayInSitu)
            , FIdx(parIdx)
        {
        }

        bool operator==(const FixedSizedArrayInSituConstIterator& other) { return FIdx == other.FIdx; }
        bool operator!=(const FixedSizedArrayInSituConstIterator& other) { return FIdx != other.FIdx; }

        void operator++(int) { FIdx++; }
        void operator++() { ++FIdx; }

        const T& operator*() { return FFixedSizedArrayInSitu->FData[FIdx]; }
        const T& operator->() = delete;

    private:
        FixedSizedArrayInSitu<T, Size>* FFixedSizedArrayInSitu;
        u32 FIdx;
    };

    class FixedSizedArrayInSituReverseIterator
    {
    public:
        FixedSizedArrayInSituReverseIterator(FixedSizedArrayInSitu<T, Size>* parFixedSizedArrayInSitu, u32 parIdx)
            : FFixedSizedArrayInSitu(parFixedSizedArrayInSitu)
            , FIdx(parIdx)
        {
        }

        bool operator==(const FixedSizedArrayInSituReverseIterator& other) { return FIdx == other.FIdx; }
        bool operator!=(const FixedSizedArrayInSituReverseIterator& other) { return FIdx != other.FIdx; }

        void operator++(int) { FIdx--; }
        void operator++() { --FIdx; }

        T& operator*() { return FFixedSizedArrayInSitu->FData[FIdx]; }
        T& operator->() = delete;

    private:
        FixedSizedArrayInSitu<T, Size>* FFixedSizedArrayInSitu;
        u32 FIdx;
    };

    class FixedSizedArrayInSituReverseConstIterator
    {
    public:
        FixedSizedArrayInSituReverseConstIterator(FixedSizedArrayInSitu<T, Size>* parFixedSizedArrayInSitu, u32 parIdx)
            : FFixedSizedArrayInSitu(parFixedSizedArrayInSitu)
            , FIdx(parIdx)
        {
        }

        bool operator==(const FixedSizedArrayInSituReverseConstIterator& other) { return FIdx == other.FIdx; }
        bool operator!=(const FixedSizedArrayInSituReverseConstIterator& other) { return FIdx != other.FIdx; }

        void operator++(int) { FIdx--; }
        void operator++() { --FIdx; }

        T& operator*() { return FFixedSizedArrayInSitu->FData[FIdx]; }
        T& operator->() = delete;

    private:
        FixedSizedArrayInSitu<T, Size>* FFixedSizedArrayInSitu;
        u32 FIdx;
    };

    friend class FixedSizedArrayInSituIterator;
    friend class FixedSizedArrayInSituConstIterator;
    friend class FixedSizedArrayInSituReverseIterator;
    friend class FixedSizedArrayInSituReverseConstIterator;

    using iterator = FixedSizedArrayInSituIterator;
    using const_iterator = FixedSizedArrayInSituConstIterator;
    using reverse_iterator = FixedSizedArrayInSituReverseIterator;
    using const_reverse_iterator = FixedSizedArrayInSituReverseConstIterator;

public:
    iterator begin() { return iterator(this, 0); }
    const_iterator cbegin() { return const_iterator(this, 0); }
    reverse_iterator rbegin() { return reverse_iterator(this, Size - 1); }
    const_reverse_iterator crbegin() { return const_reverse_iterator(this, Size - 1); }

    iterator end() { return iterator(this, Size); }
    const_iterator cend() { return const_iterator(this, Size); }
    reverse_iterator rend() { return reverse_iterator(this, (u32)-1); }
    const_reverse_iterator crend() { return const_reverse_iterator(this, (u32)-1); }

    FixedSizedArrayInSitu() { Fill(T()); }
    FixedSizedArrayInSitu(const T& val) { Fill(val); }
    FixedSizedArrayInSitu(const FixedSizedArrayInSitu<T, Size>& other) { memcpy(FData, other.FData, sizeof(FData)); }
    FixedSizedArrayInSitu(FixedSizedArrayInSitu<T, Size>&& other) { memcpy(FData, other.FData, sizeof(FData)); }

    virtual ~FixedSizedArrayInSitu() {}

    FixedSizedArrayInSitu<T, Size>& operator=(const FixedSizedArrayInSitu<T, Size>& other)
    {
        memcpy(FData, other.FData, sizeof(FData));
        return *this;
    }
    FixedSizedArrayInSitu<T, Size>& operator=(FixedSizedArrayInSitu<T, Size>&& other)
    {
        memcpy(FData, other.FData, sizeof(FData));
        return *this;
    }

    void Fill(const T& val)
    {
        for (u32 i = 0; i < Size; i++)
            FData[i] = val;
    }

    T& operator[](const u32 parIdx)
    {
        AssertRelease(parIdx < Size);
        return FData[parIdx];
    }

    const T& operator[](const u32 parIdx) const
    {
        AssertRelease(parIdx < Size);
        return FData[parIdx];
    }

    const u32 GetSize() const { return Size; }

    const T* data() const { return FData; }

private:
    alignas(T) T FData[Size];
};
} // namespace ECSEngine
