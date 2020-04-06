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
    const aiMesh* mesh0 = parMeshData->mMeshes[0];
    fileHeader.HasPositions = mesh0->HasPositions();
    if (fileHeader.HasPositions)
        fileHeader.VertexSizeInOctet += 3 * sizeof(float);

    fileHeader.NbColorChannels = mesh0->GetNumColorChannels();
    fileHeader.HasColors = fileHeader.NbColorChannels != 0;
    fileHeader.NbUVs = 0; // A IMPLEMENTER mesh->GetNumUVChannels();
    fileHeader.HasUVs = fileHeader.NbUVs != 0;

    fileHeader.HasNormals = false; // A IMPLEMENTER mesh->HasNormals();
    fileHeader.HasTangents = mesh0->HasTangentsAndBitangents();
    fileHeader.HasBinormals = fileHeader.HasTangents;

    fileHeader.VertexSizeInOctet += fileHeader.NbColorChannels * sizeof(u32);

    fileHeader.NbIndices = 0;
    forrange(i, 0, parMeshData->mNumMeshes)
    {
        const aiMesh* mesh = parMeshData->mMeshes[i];
        fileHeader.NbVertices += mesh->mNumVertices;

        forrange(j, 0, mesh->mNumFaces)
        {
            const aiFace& face = mesh->mFaces[j];
            fileHeader.NbIndices += face.mNumIndices;
        }
    }

    FOutputStream.write((c8*)&fileHeader, sizeof(MeshFileHeader));
    static_assert(sizeof(ai_real) == 4, "la taille des reels assimp est != 4");
    u32 k = 0;
    forrange(i, 0, parMeshData->mNumMeshes)
    {
        const aiMesh* mesh = parMeshData->mMeshes[i];

        forrange(l, 0, mesh->mNumVertices)
        {
            FOutputStream.write((c8*)&mesh->mVertices[l].x, 4);
            FOutputStream.write((c8*)&mesh->mVertices[l].y, 4);
            FOutputStream.write((c8*)&mesh->mVertices[l].z, 4);
            forrange(j, 0, fileHeader.NbColorChannels)
            {
                u32 r, g, b, a;
                r = (u32)(mesh->mColors[j][l].r * 255.f) & 0xFF;
                g = (u32)(mesh->mColors[j][l].g * 255.f) & 0xFF;
                b = (u32)(mesh->mColors[j][l].b * 255.f) & 0xFF;
                a = (u32)(mesh->mColors[j][l].a * 255.f) & 0xFF;
                u32 color32 = (a << 24) | (b << 16) | (g << 8) | r;
                FOutputStream.write((c8*)&color32, 4);
            }
            k++;
        }
    }

    AssertRelease(k == fileHeader.NbVertices);

    u32 vertexOffset = 0;
    std::vector<u32> indices;
    indices.reserve(fileHeader.NbIndices);
    forrange(j, 0, parMeshData->mNumMeshes)
    {
        const aiMesh* mesh = parMeshData->mMeshes[j];
        forrange(i, 0, mesh->mNumFaces)
        {

            const aiFace& face = mesh->mFaces[i];
            forrange(k, 0, face.mNumIndices) { indices.push_back(face.mIndices[k] + vertexOffset); }
        }
        vertexOffset += mesh->mNumVertices;
    }

    FOutputStream.write((c8*)indices.data(), 4u * fileHeader.NbIndices);

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