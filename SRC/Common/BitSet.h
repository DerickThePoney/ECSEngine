#pragma once

namespace ECSEngine
{
#define MEMORY_SIZE ((BitSetSize - 1) / (sizeof(BitSetType) * 8)) + 1
template<u32 BitSetSize>
class BitSet
{
    static_assert(BitSetSize > 0, "Impossible de déclarer un bitset vide !");
    using BitSetType = u32;

public:
    BitSet();
    BitSet(bool parValue);

    u32 GetValue(const u32 parBitIndex) const;
    void SetBit(const u32 parBitIndex, const bool parValue);
    void SetAllBits(const bool parValue);

    bool IsSubSetOf(const BitSet<BitSetSize>& parOther);
    bool IsSuperSetOf(const BitSet<BitSetSize>& parOther);
    bool IsSuperOrSubSetOf(const BitSet<BitSetSize>& parOther);
    bool IsDistinctFrom(const BitSet<BitSetSize>& parOther);
    bool Intersects(const BitSet<BitSetSize>& parOther);

    bool operator==(const BitSet<BitSetSize>& parOther) const;
    bool operator!=(const BitSet<BitSetSize>& parOther) const;

    const u32 size() const { return BitSetSize; }

    template<class Archive>
    void save(Archive& archive) const
    {
        const u32 thisMemSize = MEMORY_SIZE;
        archive(NAMEDPROPERTY("BitSetMemSize", thisMemSize));
        forrange(i, 0, thisMemSize) archive(FBitSet[i]);
    }

    template<class Archive>
    void load(Archive& archive)
    {
        const u32 thisMemSize = MEMORY_SIZE;
        u32 savedMemSize = 0;
        archive(NAMEDPROPERTY("BitSetMemSize", savedMemSize));

        AlwaysCheckedAssert(savedMemSize <= thisMemSize);
        forrange(i, 0, std::min(thisMemSize, savedMemSize)) archive(FBitSet[i]);
    }

private:
    void GetIndices(const u32 parBitIndex, u32& parOutBitSetBinIndex, u32& parOutBitIndexInBin) const;

private:
    BitSetType FBitSet[MEMORY_SIZE];
};

template<u32 BitSetSize>
bool BitSet<BitSetSize>::IsSuperSetOf(const BitSet<BitSetSize>& parOther)
{
    const u32 memSize = MEMORY_SIZE;
    bool res = true;
    forrange(i, 0, memSize)
    {
        const BitSetType ith = FBitSet[i] & parOther.FBitSet[i];
        const bool ithRes = (ith == parOther.FBitSet[i]);
        res = res && ithRes;
    }

    return res;
}

template<u32 BitSetSize>
bool BitSet<BitSetSize>::IsSubSetOf(const BitSet<BitSetSize>& parOther)
{
    const u32 memSize = MEMORY_SIZE;
    bool res = true;
    forrange(i, 0, memSize)
    {
        const BitSetType ith = FBitSet[i] & parOther.FBitSet[i];
        const bool ithRes = (ith == FBitSet[i]);
        res = res && ithRes;
    }

    return res;
}

template<u32 BitSetSize>
bool BitSet<BitSetSize>::Intersects(const BitSet<BitSetSize>& parOther)
{
    const u32 memSize = MEMORY_SIZE;
    bool res = true;
    forrange(i, 0, memSize)
    {
        const BitSetType ith = FBitSet[i] & parOther.FBitSet[i];
        const bool ithRes = ith != (BitSetType)0;
        res = res && ithRes;
    }

    return res;
}

template<u32 BitSetSize>
bool BitSet<BitSetSize>::IsDistinctFrom(const BitSet<BitSetSize>& parOther)
{
    const u32 memSize = MEMORY_SIZE;
    bool res = true;
    forrange(i, 0, memSize)
    {
        const BitSetType ith = FBitSet[i] & parOther.FBitSet[i];
        const bool ithRes = ith == (BitSetType)0;
        res = res && ithRes;
    }

    return res;
}

template<u32 BitSetSize>
bool BitSet<BitSetSize>::IsSuperOrSubSetOf(const BitSet<BitSetSize>& parOther)
{
    const u32 memSize = MEMORY_SIZE;
    bool res = true;
    forrange(i, 0, memSize)
    {
        const BitSetType ith = FBitSet[i] & parOther.FBitSet[i];
        const bool ithRes = (ith == FBitSet[i]) || (ith == parOther.FBitSet[i]);
        res = res && ithRes;
    }

    return res;
}

template<u32 BitSetSize>
bool BitSet<BitSetSize>::operator!=(const BitSet<BitSetSize>& parOther) const
{
    const u32 memSize = MEMORY_SIZE;
    bool res = true;
    forrange(i, 0, memSize) { res = res && ((FBitSet[i] ^ parOther.FBitSet[i]) == 0); }
    return !res;
}

template<u32 BitSetSize>
bool BitSet<BitSetSize>::operator==(const BitSet<BitSetSize>& parOther) const
{
    const u32 memSize = MEMORY_SIZE;
    bool res = true;
    forrange(i, 0, memSize) { res = res && ((FBitSet[i] ^ parOther.FBitSet[i]) == 0); }
    return res;
}

template<u32 BitSetSize>
void BitSet<BitSetSize>::GetIndices(const u32 parBitIndex, u32& parOutBitSetBinIndex, u32& parOutBitIndexInBin) const
{
    parOutBitSetBinIndex = parBitIndex / (sizeof(BitSetType) * 8);
    parOutBitIndexInBin = parBitIndex % (sizeof(BitSetType) * 8);
}

template<u32 BitSetSize>
void BitSet<BitSetSize>::SetAllBits(const bool parValue)
{
    const size_t memSize = MEMORY_SIZE;
    memset(FBitSet, (parValue) ? 0xFF : 0x00, memSize * sizeof(BitSetType));
}

template<u32 BitSetSize>
void BitSet<BitSetSize>::SetBit(const u32 parBitIndex, const bool parValue)
{
    AssertRelease(parBitIndex < BitSetSize);
    u32 bitSetBinIndex = 0, bitIndexInBin = 0;
    GetIndices(parBitIndex, bitSetBinIndex, bitIndexInBin);

    AssertRelease(bitSetBinIndex < MEMORY_SIZE);
    AssertRelease(bitIndexInBin < 8 * sizeof(BitSetType));

    FBitSet[bitSetBinIndex] = (FBitSet[bitSetBinIndex] & ~(1U << bitIndexInBin)) | (((parValue) ? 1U : 0U) << bitIndexInBin);
}

template<u32 BitSetSize>
u32 BitSet<BitSetSize>::GetValue(const u32 parBitIndex) const
{
    AssertRelease(parBitIndex < BitSetSize);
    u32 bitSetBinIndex = 0, bitIndexInBin = 0;
    GetIndices(parBitIndex, bitSetBinIndex, bitIndexInBin);

    AssertRelease(bitSetBinIndex < MEMORY_SIZE);
    AssertRelease(bitIndexInBin < 8 * sizeof(BitSetType));

    return (FBitSet[bitSetBinIndex] & (1U << bitIndexInBin)) != 0;
}

template<u32 BitSetSize>
BitSet<BitSetSize>::BitSet(bool parValue)
{
    SetAllBits(parValue);
}

template<u32 BitSetSize>
BitSet<BitSetSize>::BitSet()
{
    SetAllBits(false);
}

} // namespace ECSEngine
