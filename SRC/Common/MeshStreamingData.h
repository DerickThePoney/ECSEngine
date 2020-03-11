#pragma once
namespace ECSEngine
{
namespace Rendering
{
#pragma pack(push, r1, 1)
struct MeshFileHeader
{
    u32 MajorVersion = 0;
    u32 MinorVersion = 0;

    u32 NbVertices = 0;
    u32 VertexSizeInOctet = 0;
    bool HasPositions = false;
    bool HasColors = false;
    u32 NbColorChannels = 0;

    u32 NbIndices = 0;
};
#pragma pack(pop, r1)
} // namespace Rendering
} // namespace ECSEngine