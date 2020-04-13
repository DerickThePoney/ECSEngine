#pragma once
namespace ECSEngine
{
namespace Rendering
{
namespace VERTEX_LAYOUT_PARAMS
{
// bit positions for the hash:
// BINORMALS | TANGENTS | NORMALS | NB_UVS(3bits) | HAS_UVS | NB_COLORS(3bits) | HAS_COLORS | POSITIONS
enum Type
{
    HAS_POSTION = 0,
    HAS_COLORS = HAS_POSTION + 1, // 1 bit pour has colors et 3 pour le nombre
    HAS_UVS = HAS_COLORS + 4, // 1 bit pour has uv et 3 pour le nombre
    HAS_NORMALS = HAS_UVS + 4,
    HAS_TANGENTS = HAS_NORMALS + 1,
    HAS_BINORMALS = HAS_TANGENTS + 1,
    LENGTH = HAS_BINORMALS + 1
};
}; // namespace VERTEX_LAYOUT_PARAMS

constexpr u32 hashSize = ((VERTEX_LAYOUT_PARAMS::LENGTH == 1) ? 1 : 1 << (32 - __builtin_clz(VERTEX_LAYOUT_PARAMS::LENGTH - 1)));
constexpr u32 hashSizeByte = ((VERTEX_LAYOUT_PARAMS::LENGTH == 1) ? 1 : 1 << (32 - __builtin_clz(VERTEX_LAYOUT_PARAMS::LENGTH - 1))) / 8;

using hash_storage_type = u16;

static_assert(sizeof(hash_storage_type) == hashSizeByte);

struct MeshLayoutDescription;
struct VertexLayoutHash
{

    VertexLayoutHash();
    VertexLayoutHash(const MeshLayoutDescription& parMeshLayoutDescription);
    VertexLayoutHash(bool parPosition, u32 parNbColors, u32 parNbUvs, bool parNormals, bool parTangents, bool parBinormals);

    VertexLayoutHash(const VertexLayoutHash& parOther);
    VertexLayoutHash(VertexLayoutHash&& parOther) noexcept;

    void operator=(const VertexLayoutHash& parOther);
    void operator=(VertexLayoutHash&& parOther);

    friend std::ostream& operator<<(std::ostream& output, const VertexLayoutHash& parLayoutHash);
    friend std::istream& operator>>(std::istream& input, VertexLayoutHash& parLayoutHash);

    void SetValue(const VERTEX_LAYOUT_PARAMS::Type parValue, bool parHasValue);

    void SetColorsNb(const u32 parNbColors);
    void SetUVsNb(const u32 parNbUVs);

    hash_storage_type hash;
};

#pragma pack(push, r1, 1)
struct MeshLayoutDescription
{
    bool HasPositions = false;
    bool HasColors = false;
    u32 NbColorChannels = 0;
    bool HasUVs = false;
    u32 NbUVs = 0;
    bool HasNormals = false;
    bool HasTangents = false;
    bool HasBinormals = false;

    SERIALIZE()
    {
        NAMEDPROPERTYFIELD("HasPositions", HasPositions, true);
        NAMEDPROPERTYFIELD("HasColors", HasColors, false);
        NAMEDPROPERTYFIELD("NbColorChannels", NbColorChannels, 0);
        NAMEDPROPERTYFIELD("HasUVs", HasUVs, false);
        NAMEDPROPERTYFIELD("NbUVs", NbUVs, 0);
        NAMEDPROPERTYFIELD("HasNormals", HasNormals, false);
        NAMEDPROPERTYFIELD("HasBinormals", HasTangents, false);
        NAMEDPROPERTYFIELD("HasBinormals", HasBinormals, false);
    }
};

struct MeshFileHeader
{
    u32 MajorVersion = 0;
    u32 MinorVersion = 0;

    u32 NbVertices = 0;
    u32 VertexSizeInOctet = 0;

    MeshLayoutDescription layout;

    u32 NbIndices = 0;
};
#pragma pack(pop, r1)
} // namespace Rendering
} // namespace ECSEngine