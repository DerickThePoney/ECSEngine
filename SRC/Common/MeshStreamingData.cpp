#include "stdafx.h"

#include "MeshStreamingData.h"
namespace ECSEngine
{
namespace Rendering
{

//----------------------------------------------------------------
//          VertexLayoutHash
//----------------------------------------------------------------
VertexLayoutHash::VertexLayoutHash()
    : hash(0)
{
}

VertexLayoutHash::VertexLayoutHash(const MeshLayoutDescription& parMeshLayoutDescription)
    : hash(0)
{
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_POSTION, parMeshLayoutDescription.HasPositions);
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_COLORS, parMeshLayoutDescription.HasColors);
    SetColorsNb(parMeshLayoutDescription.NbColorChannels);
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_UVS, parMeshLayoutDescription.HasUVs);
    SetUVsNb(parMeshLayoutDescription.NbUVs);
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_NORMALS, parMeshLayoutDescription.HasNormals);
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_TANGENTS, parMeshLayoutDescription.HasTangents);
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_BINORMALS, parMeshLayoutDescription.HasBinormals);
}

VertexLayoutHash::VertexLayoutHash(bool parPosition, u32 parNbColors, u32 parNbUvs, bool parNormals, bool parTangents, bool parBinormals)
    : hash(0)
{
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_POSTION, parPosition);
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_COLORS, parNbColors > 0);
    SetColorsNb(parNbColors);
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_UVS, parNbUvs > 0);
    SetUVsNb(parNbUvs);
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_NORMALS, parNormals);
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_TANGENTS, parTangents);
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_BINORMALS, parBinormals);
}

VertexLayoutHash::VertexLayoutHash(const VertexLayoutHash& parOther)
{
    std::memcpy(&hash, &parOther.hash, hashSize);
}

VertexLayoutHash::VertexLayoutHash(VertexLayoutHash&& parOther) noexcept
{
    std::memcpy(&hash, &parOther.hash, hashSize);
}

void VertexLayoutHash::SetValue(const VERTEX_LAYOUT_PARAMS::Type parValue, bool parHasValue)
{
    AssertRelease(parValue < VERTEX_LAYOUT_PARAMS::LENGTH);
    const u32 indexInBin = parValue;
    if (parHasValue)
    {
        hash |= (1 << indexInBin);
    }
    else
    {
        hash &= ~(1 << indexInBin);
    }
}

bool VertexLayoutHash::GetValue(const VERTEX_LAYOUT_PARAMS::Type parValue) const
{
    AssertRelease(parValue < VERTEX_LAYOUT_PARAMS::LENGTH);
    const u32 indexInBin = parValue;

    return (hash & (1 << indexInBin)) > 0;
}

u32 VertexLayoutHash::GetColorsNb() const
{
    const u32 indexInBin = VERTEX_LAYOUT_PARAMS::HAS_COLORS + 1;

    const u32 nbColors = (hash & (0x3 << indexInBin)) >> indexInBin;

    AssertRelease(nbColors < 5);
    return nbColors;
}

u32 VertexLayoutHash::GetUVsNb() const
{

    const u32 indexInBin = VERTEX_LAYOUT_PARAMS::HAS_UVS + 1;

    const u32 nbUvs = (hash & (0x3 << indexInBin)) >> indexInBin;

    AssertRelease(nbUvs < 9);
    return nbUvs;
}

void VertexLayoutHash::SetColorsNb(const u32 parNbColors)
{
    AssertRelease(parNbColors < 5);
    const u32 indexInBin = VERTEX_LAYOUT_PARAMS::HAS_COLORS + 1;

    hash &= ~(0x3 << indexInBin);
    hash |= ((parNbColors & 0x3) << indexInBin);
}

void VertexLayoutHash::SetUVsNb(const u32 parNbUVs)
{
    AssertRelease(parNbUVs < 9);
    const u32 indexInBin = VERTEX_LAYOUT_PARAMS::HAS_UVS + 1;

    hash &= ~(0x3 << indexInBin);
    hash |= ((parNbUVs & 0x3) << indexInBin);
}

u32 VertexLayoutHash::GetByteSize() const
{
    u32 byteSize = 0;
    if (GetValue(VERTEX_LAYOUT_PARAMS::HAS_POSTION))
        byteSize += 3 * sizeof(float);
    if (GetValue(VERTEX_LAYOUT_PARAMS::HAS_COLORS))
        byteSize += GetColorsNb() * sizeof(u32);
    if (GetValue(VERTEX_LAYOUT_PARAMS::HAS_POSTION))
        byteSize += GetUVsNb() * 2 * sizeof(float);
    if (GetValue(VERTEX_LAYOUT_PARAMS::HAS_NORMALS))
        byteSize += 3 * sizeof(float);
    if (GetValue(VERTEX_LAYOUT_PARAMS::HAS_TANGENTS))
        byteSize += 3 * sizeof(float);
    if (GetValue(VERTEX_LAYOUT_PARAMS::HAS_BINORMALS))
        byteSize += 3 * sizeof(float);

    return byteSize;
}

void VertexLayoutHash::operator=(VertexLayoutHash&& parOther)
{
    std::memcpy(&hash, &parOther.hash, hashSize);
}

void VertexLayoutHash::operator=(const VertexLayoutHash& parOther)
{
    std::memcpy(&hash, &parOther.hash, hashSize);
}

std::ostream& operator<<(std::ostream& output, const VertexLayoutHash& parLayoutHash)
{
    output.write((c8*)&parLayoutHash.hash, hashSizeByte);
    return output;
}

std::istream& operator>>(std::istream& input, VertexLayoutHash& parLayoutHash)
{
    input.read((c8*)&parLayoutHash.hash, hashSizeByte);
    return input;
}

//----------------------------------------------------------------
//          VertexDataStream
//----------------------------------------------------------------

VertexDataStream::VertexDataStream()
    : FData(nullptr)
    , FSize(0)
    , FVertexByteSize(0)
    , FByteSize(0)
    , FCurrentVertexHead(0)
{
}

VertexDataStream::VertexDataStream(const u32 parNbVertices, const u32 parVertexByteSize, const VertexLayoutHash& hash)
    : FData(nullptr)
    , FSize(parNbVertices)
    , FVertexByteSize(parVertexByteSize)
    , FByteSize(parNbVertices * parVertexByteSize)
    , FCurrentVertexHead(0)
    , FHash(hash)
{
    FData = new c8[FByteSize];
    InitOffsetData();
}

VertexDataStream::VertexDataStream(const VertexDataStream& parOther)
{
    FSize = parOther.FSize;
    FVertexByteSize = parOther.FVertexByteSize;
    FByteSize = parOther.FByteSize;
    FCurrentVertexHead = parOther.FCurrentVertexHead;
    FHash = parOther.FHash;

    FData = new c8[FByteSize];
    memcpy(FData, parOther.FData, FByteSize);

    FOffsetMap = parOther.FOffsetMap;
}

VertexDataStream::VertexDataStream(VertexDataStream&& parOther) noexcept
{
    FSize = parOther.FSize;
    FVertexByteSize = parOther.FVertexByteSize;
    FByteSize = parOther.FByteSize;
    FCurrentVertexHead = parOther.FCurrentVertexHead;
    FHash = parOther.FHash;

    if (FData != nullptr)
        delete FData;

    FData = parOther.FData;
    parOther.FData = nullptr;

    FOffsetMap = std::move(parOther.FOffsetMap);
}

VertexDataStream::~VertexDataStream()
{
    delete FData;
    FData = nullptr;
}

void VertexDataStream::operator=(VertexDataStream&& parOther) noexcept
{
    FSize = parOther.FSize;
    FVertexByteSize = parOther.FVertexByteSize;
    FByteSize = parOther.FByteSize;
    FCurrentVertexHead = parOther.FCurrentVertexHead;
    FHash = parOther.FHash;

    if (FData != nullptr)
        delete FData;

    FData = parOther.FData;
    parOther.FData = nullptr;

    FOffsetMap = std::move(parOther.FOffsetMap);
}

void VertexDataStream::operator=(const VertexDataStream& parOther)
{
    FSize = parOther.FSize;
    FVertexByteSize = parOther.FVertexByteSize;
    FByteSize = parOther.FByteSize;
    FCurrentVertexHead = parOther.FCurrentVertexHead;
    FHash = parOther.FHash;

    FData = new c8[FByteSize];
    memcpy(FData, parOther.FData, FByteSize);

    FOffsetMap = parOther.FOffsetMap;
}

void VertexDataStream::InitOffsetData()
{
    u32 currentOffset = 0;

    // position
    if (FHash.GetValue(VERTEX_LAYOUT_PARAMS::HAS_POSTION))
    {
        TypeChannelIdPair p = { VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0 };
        OffsetByteSizePair o = { currentOffset, (u32)sizeof(glm::vec3) };
        currentOffset += o.second;
        FOffsetMap[p] = o;
    }

    // colors
    if (FHash.GetValue(VERTEX_LAYOUT_PARAMS::HAS_COLORS))
    {
        const u32 nbColors = FHash.GetColorsNb();
        AssertRelease(nbColors > 0);

        forrange(i, 0, nbColors)
        {
            TypeChannelIdPair p = { VERTEX_LAYOUT_PARAMS::HAS_COLORS, (u32)i };
            OffsetByteSizePair o = { currentOffset, (u32)sizeof(u32) };
            currentOffset += o.second;
            FOffsetMap[p] = o;
        }
    }

    // uvs
    if (FHash.GetValue(VERTEX_LAYOUT_PARAMS::HAS_UVS))
    {
        const u32 nbUvs = FHash.GetUVsNb();
        AssertRelease(nbUvs > 0);

        forrange(i, 0, nbUvs)
        {
            TypeChannelIdPair p = { VERTEX_LAYOUT_PARAMS::HAS_UVS, (u32)i };
            OffsetByteSizePair o = { currentOffset, (u32)sizeof(glm::vec2) };
            currentOffset += o.second;
            FOffsetMap[p] = o;
        }
    }

    // Normals
    if (FHash.GetValue(VERTEX_LAYOUT_PARAMS::HAS_NORMALS))
    {
        TypeChannelIdPair p = { VERTEX_LAYOUT_PARAMS::HAS_NORMALS, 0 };
        OffsetByteSizePair o = { currentOffset, (u32)sizeof(glm::vec3) };
        currentOffset += o.second;
        FOffsetMap[p] = o;
    }

    // Tangents
    if (FHash.GetValue(VERTEX_LAYOUT_PARAMS::HAS_TANGENTS))
    {
        TypeChannelIdPair p = { VERTEX_LAYOUT_PARAMS::HAS_TANGENTS, 0 };
        OffsetByteSizePair o = { currentOffset, (u32)sizeof(glm::vec3) };
        currentOffset += o.second;
        FOffsetMap[p] = o;
    }

    // Bitangents
    if (FHash.GetValue(VERTEX_LAYOUT_PARAMS::HAS_BINORMALS))
    {
        TypeChannelIdPair p = { VERTEX_LAYOUT_PARAMS::HAS_BINORMALS, 0 };
        OffsetByteSizePair o = { currentOffset, (u32)sizeof(glm::vec3) };
        currentOffset += o.second;
        FOffsetMap[p] = o;
    }

    AssertRelease(currentOffset == FVertexByteSize);
}

} // namespace Rendering
} // namespace ECSEngine