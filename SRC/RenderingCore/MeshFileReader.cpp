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
void ReadMeshImplementation(IMesh*& parMesh, std::istream& parStream)
{
    MeshFileHeader fileHeader;
    parStream.read((c8*)&fileHeader, sizeof(MeshFileHeader));

    using VertexLayout = VertexPositionColorN<1>;

    AssertRelease(sizeof(VertexLayout) == fileHeader.VertexSizeInOctet);
    std::vector<VertexLayout> vertices;
    std::vector<u32> indices;
    vertices.resize(fileHeader.NbVertices);
    indices.resize(fileHeader.NbIndices);
    forrange(i, 0, fileHeader.NbVertices)
    {
        parStream.read((c8*)&vertices[i].FPosition.x, 4);
        parStream.read((c8*)&vertices[i].FPosition.y, 4);
        parStream.read((c8*)&vertices[i].FPosition.z, 4);
        forrange(j, 0, fileHeader.NbColorChannels) { parStream.read((c8*)&vertices[i].FColor[j], 4); }
    }

    parStream.read((c8*)indices.data(), 4u * fileHeader.NbIndices);

    VertexLayoutHash hash(fileHeader);
    parMesh = MeshHelpers::CreateIMesh(hash);

    AssertRelease(parMesh != nullptr);

    parMesh->SetRawVertexData(vertices.data(), fileHeader.NbVertices * fileHeader.VertexSizeInOctet);
    parMesh->SetRawIndexData(indices.data(), fileHeader.NbIndices * 4u);
}
} // namespace

MeshFileReader::MeshFileReader()
{
}

MeshFileReader::~MeshFileReader()
{
}

void MeshFileReader::ReadMesh(IMesh*& parMesh, const std::string& parFilename)
{
    std::ifstream ifstr(parFilename.c_str(), std::ifstream::binary);
    AssertRelease(ifstr.good());
    ReadMeshImplementation(parMesh, ifstr);
}

void MeshFileReader::ReadMesh(IMesh*& parMesh, Resource& parResource)
{
    std::shared_ptr<ResourceHandle> handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&parResource);

    ResourceBuffer buff = handle->GetResourceBuffer();
    std::istream sstr(&buff, std::istream::binary);
    ReadMeshImplementation(parMesh, sstr);
}

} // namespace Rendering
} // namespace ECSEngine