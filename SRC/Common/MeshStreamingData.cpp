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
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_POSITION, parMeshLayoutDescription.HasPositions);
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_COLORS, parMeshLayoutDescription.HasColors);
    SetColorsNb(parMeshLayoutDescription.NbColorChannels);
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_UVS, parMeshLayoutDescription.HasUVs);
    SetUVsNb(parMeshLayoutDescription.NbUVs);
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_NORMALS, parMeshLayoutDescription.HasNormals);
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_TANGENTS, parMeshLayoutDescription.HasTangents);
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_BINORMALS, parMeshLayoutDescription.HasBinormals);
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_BONES, parMeshLayoutDescription.HasBones);
}

VertexLayoutHash::VertexLayoutHash(bool parPosition, u32 parNbColors, u32 parNbUvs, bool parNormals, bool parTangents, bool parBinormals, bool parBones)
    : hash(0)
{
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_POSITION, parPosition);
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_COLORS, parNbColors > 0);
    SetColorsNb(parNbColors);
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_UVS, parNbUvs > 0);
    SetUVsNb(parNbUvs);
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_NORMALS, parNormals);
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_TANGENTS, parTangents);
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_BINORMALS, parBinormals);
    SetValue(VERTEX_LAYOUT_PARAMS::HAS_BONES, parBones);
}

VertexLayoutHash::VertexLayoutHash(const VertexLayoutHash& parOther)
{
    std::memcpy(&hash, &parOther.hash, hashSizeByte);
}

bool VertexLayoutHash::operator==(const VertexLayoutHash& parOther)
{
    return (std::memcmp(&hash, &parOther.hash, hashSizeByte) == 0);
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
    if (GetValue(VERTEX_LAYOUT_PARAMS::HAS_POSITION))
        byteSize += 3 * sizeof(float);
    if (GetValue(VERTEX_LAYOUT_PARAMS::HAS_COLORS))
        byteSize += GetColorsNb() * sizeof(u32);
    if (GetValue(VERTEX_LAYOUT_PARAMS::HAS_UVS))
        byteSize += GetUVsNb() * 2 * sizeof(float);
    if (GetValue(VERTEX_LAYOUT_PARAMS::HAS_NORMALS))
        byteSize += 3 * sizeof(float);
    if (GetValue(VERTEX_LAYOUT_PARAMS::HAS_TANGENTS))
        byteSize += 3 * sizeof(float);
    if (GetValue(VERTEX_LAYOUT_PARAMS::HAS_BINORMALS))
        byteSize += 3 * sizeof(float);
    if (GetValue(VERTEX_LAYOUT_PARAMS::HAS_BONES))
        byteSize += sizeof(u32);

    return byteSize;
}

void VertexLayoutHash::operator=(const VertexLayoutHash& parOther)
{
    std::memcpy(&hash, &parOther.hash, hashSizeByte);
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
    , FCapacity(0)
    , FVertexByteSize(0)
    , FCurrentVertexHead(0)
    , FAllowResize(false)
{
}

VertexDataStream::VertexDataStream(const u32 parNbVertices, const u32 parVertexByteSize, const VertexLayoutHash& hash, bool parAllowResize)
    : FData(nullptr)
    , FCapacity(parNbVertices)
    , FVertexByteSize(parVertexByteSize)
    , FCurrentVertexHead(0)
    , FHash(hash)
    , FAllowResize(parAllowResize)
{
    FData = new c8[FCapacity * GetVertexByteSize()];
    InitOffsetData();
}

VertexDataStream::VertexDataStream(const VertexDataStream& parOther)
{
    FCapacity = parOther.FCapacity;
    FVertexByteSize = parOther.FVertexByteSize;
    FCurrentVertexHead = parOther.FCurrentVertexHead;
    FHash = parOther.FHash;
    FAllowResize = parOther.FAllowResize;

    const u32 byteSize = FCapacity * GetVertexByteSize();
    FData = new c8[byteSize];
    memcpy(FData, parOther.FData, byteSize);

    FOffsetMap = parOther.FOffsetMap;
}

VertexDataStream::~VertexDataStream()
{
    delete[] FData;
}

void VertexDataStream::operator=(const VertexDataStream& parOther)
{
    FCapacity = parOther.FCapacity;
    FVertexByteSize = parOther.FVertexByteSize;
    FCurrentVertexHead = parOther.FCurrentVertexHead;
    FHash = parOther.FHash;
    FAllowResize = parOther.FAllowResize;

    const u32 byteSize = FCapacity * GetVertexByteSize();
    FData = new c8[byteSize];
    memcpy(FData, parOther.FData, byteSize);

    FOffsetMap = parOther.FOffsetMap;
}

void VertexDataStream::InitOffsetData()
{
    u32 currentOffset = 0;

    // position
    if (FHash.GetValue(VERTEX_LAYOUT_PARAMS::HAS_POSITION))
    {
        TypeChannelIdPair p = { VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0 };
        OffsetByteSizePair o = { currentOffset, (u32)sizeof(vec3) };
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
            OffsetByteSizePair o = { currentOffset, (u32)sizeof(vec2) };
            currentOffset += o.second;
            FOffsetMap[p] = o;
        }
    }

    // Normals
    if (FHash.GetValue(VERTEX_LAYOUT_PARAMS::HAS_NORMALS))
    {
        TypeChannelIdPair p = { VERTEX_LAYOUT_PARAMS::HAS_NORMALS, 0 };
        OffsetByteSizePair o = { currentOffset, (u32)sizeof(vec3) };
        currentOffset += o.second;
        FOffsetMap[p] = o;
    }

    // Tangents
    if (FHash.GetValue(VERTEX_LAYOUT_PARAMS::HAS_TANGENTS))
    {
        TypeChannelIdPair p = { VERTEX_LAYOUT_PARAMS::HAS_TANGENTS, 0 };
        OffsetByteSizePair o = { currentOffset, (u32)sizeof(vec3) };
        currentOffset += o.second;
        FOffsetMap[p] = o;
    }

    // Bitangents
    if (FHash.GetValue(VERTEX_LAYOUT_PARAMS::HAS_BINORMALS))
    {
        TypeChannelIdPair p = { VERTEX_LAYOUT_PARAMS::HAS_BINORMALS, 0 };
        OffsetByteSizePair o = { currentOffset, (u32)sizeof(vec3) };
        currentOffset += o.second;
        FOffsetMap[p] = o;
    }

    // Bones
    if (FHash.GetValue(VERTEX_LAYOUT_PARAMS::HAS_BONES))
    {
        TypeChannelIdPair p = { VERTEX_LAYOUT_PARAMS::HAS_BONES, 0 };
        OffsetByteSizePair o = { currentOffset, (u32)sizeof(u32) };
        currentOffset += o.second;
        FOffsetMap[p] = o;
    }

    AssertRelease(currentOffset == FVertexByteSize);
}

void VertexDataStream::GrowIFN()
{
    if (!FAllowResize)
        return;

    if (FCurrentVertexHead < FCapacity)
        return;

    const u32 oldByteSize = FCapacity * GetVertexByteSize();
    FCapacity = 2 * FCapacity;
    const u32 newByteSize = FCapacity * GetVertexByteSize();

    c8* newData = new c8[newByteSize];
    memcpy(newData, FData, std::min(oldByteSize, newByteSize));
    delete[] FData;
    FData = newData;
}

} // namespace Rendering
} // namespace ECSEngine
