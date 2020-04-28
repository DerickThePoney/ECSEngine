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

    VertexLayoutHash hash(fileHeader.layout);

    VertexDataStream stream(fileHeader.NbVertices, fileHeader.VertexSizeInOctet, hash);

    std::vector<u32> indices;
    indices.resize(fileHeader.NbIndices);
    forrange(i, 0, fileHeader.NbVertices)
    {
        if (fileHeader.layout.HasPositions)
        {
            glm::vec3 data(0.0f);
            parStream.read((c8*)&data.x, 4);
            parStream.read((c8*)&data.y, 4);
            parStream.read((c8*)&data.z, 4);
            stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, data);
        }

        forrange(j, 0, fileHeader.layout.NbColorChannels)
        {
            u32 color = 0;
            parStream.read((c8*)&color, 4);
            stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, color);
        }

        forrange(j, 0, fileHeader.layout.NbUVs)
        {
            glm::vec2 uv(0.0f);
            parStream.read((c8*)&uv.x, 4);
            parStream.read((c8*)&uv.y, 4);
            stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_UVS, 0, uv);
        }

        if (fileHeader.layout.HasNormals)
        {
            glm::vec3 data(0.0f);
            parStream.read((c8*)&data.x, 4);
            parStream.read((c8*)&data.y, 4);
            parStream.read((c8*)&data.z, 4);
            stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_NORMALS, 0, data);
        }

        if (fileHeader.layout.HasTangents)
        {
            glm::vec3 data(0.0f);
            parStream.read((c8*)&data.x, 4);
            parStream.read((c8*)&data.y, 4);
            parStream.read((c8*)&data.z, 4);
            stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_TANGENTS, 0, data);
        }

        if (fileHeader.layout.HasTangents)
        {
            glm::vec3 data(0.0f);
            parStream.read((c8*)&data.x, 4);
            parStream.read((c8*)&data.y, 4);
            parStream.read((c8*)&data.z, 4);
            stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_TANGENTS, 0, data);
        }

        stream.Advance();
    }

    parStream.read((c8*)indices.data(), 4u * fileHeader.NbIndices);

    parMesh = MeshHelpers::CreateIMesh(hash);

    AssertRelease(parMesh != nullptr);

    parMesh->SetRawVertexData(stream);
    parMesh->SetRawIndexData(indices.data(), fileHeader.NbIndices * 4u);
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

void MeshFileReader::ReadMesh(Mesh*& parMesh, Resource& parResource)
{
    std::shared_ptr<ResourceHandle> handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&parResource);

    ResourceBuffer buff = handle->GetResourceBuffer();
    std::istream sstr(&buff, std::istream::binary);
    ReadMeshImplementation(parMesh, sstr);
}

} // namespace Rendering
} // namespace ECSEngine