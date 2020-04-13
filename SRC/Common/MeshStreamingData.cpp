#include "stdafx.h"

#include "MeshStreamingData.h"
namespace ECSEngine
{
namespace Rendering
{

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

void VertexLayoutHash::SetColorsNb(const u32 parNbColors)
{
    AssertRelease(parNbColors < (1 << 3));
    const u32 indexInBin = VERTEX_LAYOUT_PARAMS::HAS_COLORS + 1;

    hash &= ~(0x3 << indexInBin);
    hash |= ((parNbColors & 0x3) << indexInBin);
}

void VertexLayoutHash::SetUVsNb(const u32 parNbUVs)
{
    AssertRelease(parNbUVs < (1 << 3));
    const u32 indexInBin = VERTEX_LAYOUT_PARAMS::HAS_UVS + 1;

    hash &= ~(0x3 << indexInBin);
    hash |= ((parNbUVs & 0x3) << indexInBin);
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
} // namespace Rendering
} // namespace ECSEngine