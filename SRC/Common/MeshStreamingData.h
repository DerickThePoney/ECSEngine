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
    HAS_UVS = HAS_COLORS + 4, // 1 bit pour has uv et 3 pour le nombre (max 4 couleurs)
    HAS_NORMALS = HAS_UVS + 5, // 1 bit pour has uv et 4 pour le nombre (max 8 uvs)
    HAS_TANGENTS = HAS_NORMALS + 1,
    HAS_BINORMALS = HAS_TANGENTS + 1,
    HAS_BONES = HAS_BINORMALS + 1,
    LENGTH = HAS_BONES + 1 // HAS_BINORMALS + 1  // Recompute leadHashSizeLeadingZeros if you change this
                           // [__builtin_clz(VERTEX_LAYOUT_PARAMS::LENGTH - 1)]
};
constexpr u32 leadHashSizeLeadingZeros = 28; // --> Apparently support for __builtin_clz got dropped at some point so take extracare with this kind of shit
}; // namespace VERTEX_LAYOUT_PARAMS

//----------------------------------------------------------------
//          VertexLayoutHash
//----------------------------------------------------------------

constexpr u32 hashSize = ((VERTEX_LAYOUT_PARAMS::LENGTH == 1) ? 1 : 1 << (32 - VERTEX_LAYOUT_PARAMS::leadHashSizeLeadingZeros));
constexpr u32 hashSizeByte = ((VERTEX_LAYOUT_PARAMS::LENGTH == 1) ? 1 : 1 << (32 - VERTEX_LAYOUT_PARAMS::leadHashSizeLeadingZeros)) / 8;

using hash_storage_type = u16;

static_assert(sizeof(hash_storage_type) == hashSizeByte);

struct MeshLayoutDescription;
struct VertexLayoutHash
{

    VertexLayoutHash();
    VertexLayoutHash(const MeshLayoutDescription& parMeshLayoutDescription);
    VertexLayoutHash(bool parPosition, u32 parNbColors, u32 parNbUvs, bool parNormals, bool parTangents, bool parBinormals, bool parBones);

    VertexLayoutHash(const VertexLayoutHash& parOther);
    VertexLayoutHash(VertexLayoutHash&& parOther) = delete;

    void operator=(const VertexLayoutHash& parOther);
    void operator=(VertexLayoutHash&& parOther) = delete;

    friend std::ostream& operator<<(std::ostream& output, const VertexLayoutHash& parLayoutHash);
    friend std::istream& operator>>(std::istream& input, VertexLayoutHash& parLayoutHash);

    void SetValue(const VERTEX_LAYOUT_PARAMS::Type parValue, bool parHasValue);
    bool GetValue(const VERTEX_LAYOUT_PARAMS::Type parValue) const;

    u32 GetColorsNb() const;
    u32 GetUVsNb() const;
    void SetColorsNb(const u32 parNbColors);
    void SetUVsNb(const u32 parNbUVs);
    u32 GetByteSize() const;

    hash_storage_type hash;
};

//----------------------------------------------------------------
//          VertexDataStream
//----------------------------------------------------------------
class VertexDataStream
{
    using TypeChannelIdPair = std::pair<VERTEX_LAYOUT_PARAMS::Type, u32>;
    using OffsetByteSizePair = std::pair<u32, u32>;

public:
    VertexDataStream();
    VertexDataStream(const u32 parNbVertices, const u32 parVertexByteSize, const VertexLayoutHash& hash);
    VertexDataStream(const VertexDataStream& parOther);
    VertexDataStream(VertexDataStream&& parOther) = delete;
    ~VertexDataStream();

    void operator=(const VertexDataStream& parOther);
    void operator=(VertexDataStream&& parOther) = delete;

    u32 GetSize() const { return FSize; }
    u32 GetVertexByteSize() const { return FVertexByteSize; }
    u32 GetByteSize() const { return FByteSize; }
    const VertexLayoutHash& GetHash() const { return FHash; }

    template<typename T>
    void PushData(const VERTEX_LAYOUT_PARAMS::Type parType, const u32 parChannel, const T& parData)
    {
        AssertRelease(FCurrentVertexHead < FSize);
        const TypeChannelIdPair p = { parType, parChannel };
        AssertRelease(FOffsetMap.find(p) != FOffsetMap.end());
        const OffsetByteSizePair& o = FOffsetMap[p];
        AssertRelease((u32)sizeof(T) == o.second);
        const u32 ptrOffset = FCurrentVertexHead * FVertexByteSize + o.first;
        AssertRelease(ptrOffset < FByteSize);
        AssertRelease((ptrOffset + o.second) <= FByteSize);
        c8* writePosition = FData + ptrOffset;
        memcpy(writePosition, &parData, sizeof(T));

#ifdef PERFORM_SECURITY_CHECKS
        const T writenValue = *reinterpret_cast<T*>(writePosition);
        AssertRelease(writenValue == parData);
#endif
    }

    template<typename T>
    void SetValue(const VERTEX_LAYOUT_PARAMS::Type parType, const u32 parChannel, const T& parData, const u32 parVertexId)
    {
        AssertRelease(parVertexId < FSize);
        const TypeChannelIdPair p = { parType, parChannel };
        AssertRelease(FOffsetMap.find(p) != FOffsetMap.end());
        const OffsetByteSizePair& o = FOffsetMap[p];
        AssertRelease((u32)sizeof(T) == o.second);
        const u32 ptrOffset = parVertexId * FVertexByteSize + o.first;
        AssertRelease(ptrOffset < FByteSize);
        AssertRelease((ptrOffset + o.second) <= FByteSize);
        c8* writePosition = FData + ptrOffset;
        memcpy(writePosition, &parData, sizeof(T));

#ifdef PERFORM_SECURITY_CHECKS
        const T writenValue = *reinterpret_cast<T*>(writePosition);
        AssertRelease(writenValue == parData);
#endif
    }

    template<typename T>
    const T& GetValue(const VERTEX_LAYOUT_PARAMS::Type parType, const u32 parChannel, const u32 parVertexId)
    {
        AssertRelease(parVertexId < FSize);
        const TypeChannelIdPair p = { parType, parChannel };
        AssertRelease(FOffsetMap.find(p) != FOffsetMap.end());
        const OffsetByteSizePair& o = FOffsetMap[p];
        AssertRelease((u32)sizeof(T) == o.second);
        const u32 ptrOffset = parVertexId * FVertexByteSize + o.first;
        AssertRelease(ptrOffset < FByteSize);
        AssertRelease((ptrOffset + o.second) <= FByteSize);
        c8* writePosition = FData + ptrOffset;

        return *reinterpret_cast<T*>(writePosition);
    }

    void Advance()
    {
        FCurrentVertexHead++;
        AssertRelease(FCurrentVertexHead <= FSize);
    }

    const void* GetData() const
    {
        AssertRelease(FData != nullptr);
        return FData;
    }

    void* GetData()
    {
        AssertRelease(FData != nullptr);
        return FData;
    }

private:
    void InitOffsetData();

private:
    std::map<TypeChannelIdPair, OffsetByteSizePair> FOffsetMap;

    c8* FData;
    u32 FVertexByteSize;
    u32 FByteSize;
    u32 FSize;

    u32 FCurrentVertexHead;

    VertexLayoutHash FHash;
};

//----------------------------------------------------------------
//          MeshLayoutDescription
//----------------------------------------------------------------
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
    bool HasBones = false;

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
        NAMEDPROPERTYFIELD("HasBones", HasBones, false);
    }
};

//----------------------------------------------------------------
//          MeshFileHeader
//----------------------------------------------------------------
namespace MagicStuff
{
constexpr u32 MajorVersion = 0;
constexpr u32 MinorVersion = 3;
} // namespace MagicStuff

struct MeshFileHeader
{
    u32 MajorVersion = 0;
    u32 MinorVersion = 0;

    u32 NbVertices = 0;
    u32 VertexSizeInOctet = 0;

    MeshLayoutDescription layout;

    u32 NbIndices = 0;

    u32 NbNodesInHierarchy = 0;
};

struct HierarchyNode
{
    u8 Idx = -1;
    u8 Parent = -1;
    const char* Name = nullptr;
    glm::mat4 LocalTransform = glm::identity<glm::mat4>();
};
#pragma pack(pop, r1)
} // namespace Rendering
} // namespace ECSEngine
