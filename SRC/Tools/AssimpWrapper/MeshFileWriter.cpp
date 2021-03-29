#include "stdafx.h"

#include "MeshFileWriter.h"

#include "Common/MeshStreamingData.h"

namespace ECSEngine
{
namespace Rendering
{

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
    fileHeader.layout.HasPositions = mesh0->HasPositions();

    fileHeader.layout.NbColorChannels = mesh0->GetNumColorChannels();
    fileHeader.layout.HasColors = fileHeader.layout.NbColorChannels != 0;
    fileHeader.layout.NbUVs = mesh0->GetNumUVChannels();
    fileHeader.layout.HasUVs = fileHeader.layout.NbUVs != 0;

    fileHeader.layout.HasNormals = mesh0->HasNormals();
    fileHeader.layout.HasTangents = mesh0->HasTangentsAndBitangents();
    fileHeader.layout.HasBinormals = fileHeader.layout.HasTangents;

    if (fileHeader.layout.HasPositions)
        fileHeader.VertexSizeInOctet += 3 * sizeof(float);

    fileHeader.VertexSizeInOctet += fileHeader.layout.NbColorChannels * sizeof(u32);
    fileHeader.VertexSizeInOctet += fileHeader.layout.NbUVs * 2 * sizeof(float);

    if (fileHeader.layout.HasNormals)
        fileHeader.VertexSizeInOctet += 3 * sizeof(float);
    if (fileHeader.layout.HasTangents)
        fileHeader.VertexSizeInOctet += 3 * sizeof(float);
    if (fileHeader.layout.HasTangents)
        fileHeader.VertexSizeInOctet += 3 * sizeof(float);

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
            if (fileHeader.layout.HasPositions)
            {
                FOutputStream.write((c8*)&mesh->mVertices[l].x, 4);
                FOutputStream.write((c8*)&mesh->mVertices[l].y, 4);
                FOutputStream.write((c8*)&mesh->mVertices[l].z, 4);
            }

            forrange(j, 0, fileHeader.layout.NbColorChannels)
            {
                u32 r, g, b, a;
                r = (u32)(mesh->mColors[j][l].r * 255.f) & 0xFF;
                g = (u32)(mesh->mColors[j][l].g * 255.f) & 0xFF;
                b = (u32)(mesh->mColors[j][l].b * 255.f) & 0xFF;
                a = (u32)(mesh->mColors[j][l].a * 255.f) & 0xFF;
                u32 color32 = (a << 24) | (b << 16) | (g << 8) | r;
                FOutputStream.write((c8*)&color32, 4);
            }

            forrange(j, 0, fileHeader.layout.NbUVs)
            {
                FOutputStream.write((c8*)&mesh->mTextureCoords[j][l].x, 4);
                FOutputStream.write((c8*)&mesh->mTextureCoords[j][l].y, 4);
            }

            if (fileHeader.layout.HasNormals)
            {
                FOutputStream.write((c8*)&mesh->mNormals[l].x, 4);
                FOutputStream.write((c8*)&mesh->mNormals[l].y, 4);
                FOutputStream.write((c8*)&mesh->mNormals[l].z, 4);
            }

            if (fileHeader.layout.HasTangents)
            {
                FOutputStream.write((c8*)&mesh->mTangents[l].x, 4);
                FOutputStream.write((c8*)&mesh->mTangents[l].y, 4);
                FOutputStream.write((c8*)&mesh->mTangents[l].z, 4);
            }

            if (fileHeader.layout.HasBinormals)
            {
                FOutputStream.write((c8*)&mesh->mBitangents[l].x, 4);
                FOutputStream.write((c8*)&mesh->mBitangents[l].y, 4);
                FOutputStream.write((c8*)&mesh->mBitangents[l].z, 4);
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
}

} // namespace Rendering
} // namespace ECSEngine
