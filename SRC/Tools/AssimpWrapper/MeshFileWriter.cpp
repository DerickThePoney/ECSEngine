#include "stdafx.h"

#include "MeshFileWriter.h"

#include "Common/MeshStreamingData.h"

namespace ECSEngine
{
namespace Rendering
{
namespace MagicStuff
{
constexpr u32 MajorVersion = 0;
constexpr u32 MinorVersion = 0;
} // namespace MagicStuff
MeshFileWriter::MeshFileWriter(const std::string& parFilename)
{
    FOutputStream.open(parFilename.c_str(), std::ofstream::binary);
    AssertRelease(FOutputStream.is_open());
    AssertRelease(FOutputStream.good());
}

MeshFileWriter::~MeshFileWriter()
{
    FOutputStream.flush();
    FOutputStream.close();
}

void MeshFileWriter::operator<<(const aiScene* parMeshData)
{
    AssertRelease(FOutputStream.is_open());
    AssertRelease(FOutputStream.good());
    MeshFileHeader fileHeader;

    fileHeader.MajorVersion = MagicStuff::MajorVersion;
    fileHeader.MinorVersion = MagicStuff::MinorVersion;
    const aiMesh* mesh = parMeshData->mMeshes[0];
    fileHeader.HasPositions = mesh->HasPositions();
    if (fileHeader.HasPositions)
        fileHeader.VertexSizeInOctet += 3 * sizeof(float);

    fileHeader.NbColorChannels = mesh->GetNumColorChannels();
    fileHeader.HasColors = fileHeader.NbColorChannels != 0;
    fileHeader.NbUVs = 0; // A IMPLEMENTER mesh->GetNumUVChannels();
    fileHeader.HasUVs = fileHeader.NbUVs != 0;

    fileHeader.HasNormals = false; // A IMPLEMENTER mesh->HasNormals();
    fileHeader.HasTangents = mesh->HasTangentsAndBitangents();
    fileHeader.HasBinormals = fileHeader.HasTangents;

    fileHeader.VertexSizeInOctet += fileHeader.NbColorChannels * sizeof(u32);

    fileHeader.NbVertices = mesh->mNumVertices;

    fileHeader.NbIndices = 0;
    forrange(i, 0, mesh->mNumFaces)
    {
        const aiFace& face = mesh->mFaces[i];
        fileHeader.NbIndices += face.mNumIndices;
    }

    FOutputStream.write((c8*)&fileHeader, sizeof(MeshFileHeader));
    static_assert(sizeof(ai_real) == 4, "la taille des reels assimp est != 4");
    forrange(i, 0, fileHeader.NbVertices)
    {
        FOutputStream.write((c8*)&mesh->mVertices[i].x, 4);
        FOutputStream.write((c8*)&mesh->mVertices[i].y, 4);
        FOutputStream.write((c8*)&mesh->mVertices[i].z, 4);
        forrange(j, 0, fileHeader.NbColorChannels)
        {
            u32 r, g, b, a;
            r = (u32)(mesh->mColors[j][i].r * 255.f) & 0xFF;
            g = (u32)(mesh->mColors[j][i].g * 255.f) & 0xFF;
            b = (u32)(mesh->mColors[j][i].b * 255.f) & 0xFF;
            a = (u32)(mesh->mColors[j][i].a * 255.f) & 0xFF;
            u32 color32 = (a << 24) | (b << 16) | (g << 8) | r;
            FOutputStream.write((c8*)&color32, 4);
        }
    }

    forrange(i, 0, mesh->mNumFaces)
    {
        const aiFace& face = mesh->mFaces[i];
        FOutputStream.write((c8*)face.mIndices, 4u * face.mNumIndices);
    }

    /*   FOutputStream.write((c8*)vertexData, fileHeader.VertexSizeInOctet * fileHeader.NbVertices);
       FOutputStream.write((c8*)indexData, fileHeader.IndexSizeInOctets * fileHeader.NbIndices);*/
}

// void MeshFileWriter::operator<<(const IMesh* parMeshData)
//{
//    AssertRelease(FOutputStream.is_open());
//    AssertRelease(FOutputStream.good());
//    MeshFileHeader fileHeader;
//    void* vertexData = nullptr;
//    void* indexData = nullptr;
//
//    fileHeader.MajorVersion = MagicStuff::MajorVersion;
//    fileHeader.MinorVersion = MagicStuff::MinorVersion;
//
//    parMeshData->FillMeshFileHeader(fileHeader, vertexData, indexData);
//
//    AssertRelease(vertexData != nullptr);
//    AssertRelease(indexData != nullptr);
//
//    FOutputStream.write((c8*)&fileHeader, sizeof(MeshFileHeader));
//
//    FOutputStream.write((c8*)vertexData, fileHeader.VertexSizeInOctet * fileHeader.NbVertices);
//    FOutputStream.write((c8*)indexData, fileHeader.IndexSizeInOctets * fileHeader.NbIndices);
//}

} // namespace Rendering
} // namespace ECSEngine