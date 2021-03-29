#include "stdafx.h"

#include "MeshFileReader.h"

#include "Common/MeshStreamingData.h"
#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
#include "Mesh.h"
#include "MeshUtils.h"
#include "VertexLayout.h"
namespace ECSEngine
{
namespace Rendering
{

namespace
{
void ReadMeshImplementation(Mesh*& parMesh, std::istream& parStream)
{
    MeshFileHeader fileHeader;
    parStream.read((c8*)&fileHeader, sizeof(MeshFileHeader));

    AssertRelease(fileHeader.MajorVersion == MagicStuff::MajorVersion);
    AssertRelease(fileHeader.MinorVersion == MagicStuff::MinorVersion);

    VertexLayoutHash hash(fileHeader.layout);

    VertexDataStream vertexDataStream(fileHeader.NbVertices, fileHeader.VertexSizeInOctet, hash);

    std::vector<u32> indices;
    glm::vec3 verticesGravityCenter = glm::vec3(0.0f);
    indices.resize(fileHeader.NbIndices);
    forrange(i, 0, fileHeader.NbVertices)
    {
        if (fileHeader.layout.HasPositions)
        {
            glm::vec3 data(0.0f);
            parStream.read((c8*)&data.x, 4);
            parStream.read((c8*)&data.y, 4);
            parStream.read((c8*)&data.z, 4);
            verticesGravityCenter += data;
            vertexDataStream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, data);
        }

        forrange(j, 0, fileHeader.layout.NbColorChannels)
        {
            u32 color = 0;
            parStream.read((c8*)&color, 4);
            vertexDataStream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, color);
        }

        forrange(j, 0, fileHeader.layout.NbUVs)
        {
            glm::vec2 uv(0.0f);
            parStream.read((c8*)&uv.x, 4);
            parStream.read((c8*)&uv.y, 4);
            vertexDataStream.PushData(VERTEX_LAYOUT_PARAMS::HAS_UVS, 0, uv);
        }

        if (fileHeader.layout.HasNormals)
        {
            glm::vec3 data(0.0f);
            parStream.read((c8*)&data.x, 4);
            parStream.read((c8*)&data.y, 4);
            parStream.read((c8*)&data.z, 4);
            vertexDataStream.PushData(VERTEX_LAYOUT_PARAMS::HAS_NORMALS, 0, data);
        }

        if (fileHeader.layout.HasTangents)
        {
            glm::vec3 data(0.0f);
            parStream.read((c8*)&data.x, 4);
            parStream.read((c8*)&data.y, 4);
            parStream.read((c8*)&data.z, 4);
            vertexDataStream.PushData(VERTEX_LAYOUT_PARAMS::HAS_TANGENTS, 0, data);
        }

        if (fileHeader.layout.HasTangents)
        {
            glm::vec3 data(0.0f);
            parStream.read((c8*)&data.x, 4);
            parStream.read((c8*)&data.y, 4);
            parStream.read((c8*)&data.z, 4);
            vertexDataStream.PushData(VERTEX_LAYOUT_PARAMS::HAS_TANGENTS, 0, data);
        }

        vertexDataStream.Advance();
    }

    verticesGravityCenter /= fileHeader.NbVertices;
    float radius = 0.0f;
    if (fileHeader.layout.HasPositions)
    {
        forrange(i, 0, fileHeader.NbVertices)
        {
            const glm::vec3 currentVertex = vertexDataStream.GetValue<glm::vec3>(Rendering::VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, (u32)i);
            radius = std::max(radius, glm::length2(currentVertex - verticesGravityCenter));
        }
    }

    radius = std::sqrt(radius);

    parStream.read((c8*)indices.data(), 4u * fileHeader.NbIndices);

    parMesh = MeshHelpers::CreateIMesh(hash);

    AssertRelease(parMesh != nullptr);

    parMesh->SetRawVertexData(vertexDataStream);
    parMesh->SetRawIndexData(indices.data(), fileHeader.NbIndices * 4u);
    parMesh->SetBoundingCircle(glm::vec4(verticesGravityCenter, radius));
}
} // namespace

MeshFileReader::MeshFileReader()
{
}

MeshFileReader::~MeshFileReader()
{
}

void MeshFileReader::ReadMesh(Mesh*& parMesh, const std::string& parFilename)
{
    std::ifstream ifstr(parFilename.c_str(), std::ifstream::binary);
    AssertRelease(ifstr.good());
    ReadMeshImplementation(parMesh, ifstr);
}

void MeshFileReader::ReadMesh(Mesh*& parMesh, const Resource& parResource)
{
    std::shared_ptr<ResourceHandle> handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&parResource);

    ResourceBuffer buff = handle->GetResourceBuffer();
    std::istream sstr(&buff, std::istream::binary);
    ReadMeshImplementation(parMesh, sstr);
}

} // namespace Rendering
} // namespace ECSEngine
