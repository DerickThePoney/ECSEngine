#include "stdafx.h"

#include "MeshFileReader.h"

#include "Common/MeshStreamingData.h"
#include "Mesh.h"
#include "MeshUtils.h"
#include "VertexLayout.h"
namespace ECSEngine
{
namespace Rendering
{

MeshFileReader::MeshFileReader(const std::string& parFilename)
{
    FInputFile.open(parFilename.c_str(), std::ifstream::binary);
    AssertRelease(FInputFile.good());
}

MeshFileReader::~MeshFileReader()
{
    FInputFile.close();
}

void MeshFileReader::operator>>(IMesh*& parMesh)
{
    MeshFileHeader fileHeader;
    FInputFile.read((c8*)&fileHeader, sizeof(MeshFileHeader));

    using VertexLayout = VertexPositionColorN<1>;

    AssertRelease(sizeof(VertexLayout) == fileHeader.VertexSizeInOctet);
    std::vector<VertexLayout> vertices;
    std::vector<u32> indices;
    vertices.resize(fileHeader.NbVertices);
    indices.resize(fileHeader.NbIndices);
    forrange(i, 0, fileHeader.NbVertices)
    {
        FInputFile.read((c8*)&vertices[i].FPosition.x, 4);
        FInputFile.read((c8*)&vertices[i].FPosition.y, 4);
        FInputFile.read((c8*)&vertices[i].FPosition.z, 4);
        forrange(j, 0, fileHeader.NbColorChannels) { FInputFile.read((c8*)&vertices[i].FColor[j], 4); }
    }

    FInputFile.read((c8*)indices.data(), 4u * fileHeader.NbIndices);

    VertexLayoutHash hash(fileHeader);
    parMesh = MeshHelpers::CreateIMesh(hash);

    AssertRelease(parMesh != nullptr);

    parMesh->SetRawVertexData(vertices.data(), fileHeader.NbVertices * fileHeader.VertexSizeInOctet);
    parMesh->SetRawIndexData(indices.data(), fileHeader.NbIndices * 4u);
}

} // namespace Rendering
} // namespace ECSEngine