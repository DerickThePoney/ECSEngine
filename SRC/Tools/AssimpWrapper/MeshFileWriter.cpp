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

void ReadHierarchy(const aiNode* parNode, const u8 parParentIdx, std::vector<HierarchyNode>& parHierarchy)
{
    HierarchyNode node;
    node.Idx = (u8)parHierarchy.size();
    node.Parent = parParentIdx;
    node.Name = parNode->mName.C_Str();

    const aiMatrix4x4& mat = parNode->mTransformation;
    node.InverseLocalTransform[0][0] = mat.a1;
    node.InverseLocalTransform[0][1] = mat.b1;
    node.InverseLocalTransform[0][2] = mat.c1;
    node.InverseLocalTransform[0][3] = mat.d1;

    node.InverseLocalTransform[1][0] = mat.a2;
    node.InverseLocalTransform[1][1] = mat.b2;
    node.InverseLocalTransform[1][2] = mat.c2;
    node.InverseLocalTransform[1][3] = mat.d2;

    node.InverseLocalTransform[2][0] = mat.a3;
    node.InverseLocalTransform[2][1] = mat.b3;
    node.InverseLocalTransform[2][2] = mat.c3;
    node.InverseLocalTransform[2][3] = mat.d3;

    node.InverseLocalTransform[3][0] = mat.a4;
    node.InverseLocalTransform[3][1] = mat.b4;
    node.InverseLocalTransform[3][2] = mat.c4;
    node.InverseLocalTransform[3][3] = mat.d4;

    const u8 parent = node.Idx;
    parHierarchy.push_back(node);
    forrange(i, 0, parNode->mNumChildren) { ReadHierarchy(parNode->mChildren[i], parent, parHierarchy); }
}

void ReadHierarchyMesh(const aiScene* parScene,
      const aiNode* parNode,
      const MeshFileHeader& parFileHeader,
      const std::vector<HierarchyNode>& parHierarchy,
      u32& outCurrentNodeIdx,
      std::ofstream& parOutputStream,
      u32& outNbVertices,
      const aiMatrix4x4 parParentTransform = aiMatrix4x4())
{
    AssertRelease(parNode->mName.C_Str() == parHierarchy[outCurrentNodeIdx].Name);

    const float nodeIdx = (float)outCurrentNodeIdx;
    const aiMatrix4x4 transform = parParentTransform * parNode->mTransformation;
    forrange(i, 0, parNode->mNumMeshes)
    {
        const aiMesh* mesh = parScene->mMeshes[parNode->mMeshes[i]];

        forrange(l, 0, mesh->mNumVertices)
        {
            if (parFileHeader.layout.HasPositions)
            {
                const aiVector3D vertex = transform * mesh->mVertices[l];
                parOutputStream.write((c8*)&vertex.x, 4);
                parOutputStream.write((c8*)&vertex.y, 4);
                parOutputStream.write((c8*)&vertex.z, 4);
            }

            forrange(j, 0, parFileHeader.layout.NbColorChannels)
            {
                u32 r, g, b, a;
                r = (u32)(mesh->mColors[j][l].r * 255.f) & 0xFF;
                g = (u32)(mesh->mColors[j][l].g * 255.f) & 0xFF;
                b = (u32)(mesh->mColors[j][l].b * 255.f) & 0xFF;
                a = (u32)(mesh->mColors[j][l].a * 255.f) & 0xFF;
                u32 color32 = (a << 24) | (b << 16) | (g << 8) | r;
                parOutputStream.write((c8*)&color32, 4);
            }

            forrange(j, 0, parFileHeader.layout.NbUVs)
            {
                parOutputStream.write((c8*)&mesh->mTextureCoords[j][l].x, 4);
                parOutputStream.write((c8*)&mesh->mTextureCoords[j][l].y, 4);
            }

            if (parFileHeader.layout.HasNormals)
            {
                const aiVector3D normal = transform * mesh->mNormals[l];
                parOutputStream.write((c8*)&normal.x, 4);
                parOutputStream.write((c8*)&normal.y, 4);
                parOutputStream.write((c8*)&normal.z, 4);
            }

            if (parFileHeader.layout.HasTangents)
            {
                const aiVector3D tangent = transform * mesh->mTangents[l];
                parOutputStream.write((c8*)&tangent.x, 4);
                parOutputStream.write((c8*)&tangent.y, 4);
                parOutputStream.write((c8*)&tangent.z, 4);
            }

            if (parFileHeader.layout.HasBinormals)
            {
                const aiVector3D binormal = transform * mesh->mBitangents[l];
                parOutputStream.write((c8*)&binormal.x, 4);
                parOutputStream.write((c8*)&binormal.y, 4);
                parOutputStream.write((c8*)&binormal.z, 4);
            }

            parOutputStream.write((c8*)&nodeIdx, sizeof(float));

            outNbVertices++;
        }
    }

    forrange(i, 0, parNode->mNumChildren)
    {
        ReadHierarchyMesh(parScene, parNode->mChildren[i], parFileHeader, parHierarchy, ++outCurrentNodeIdx, parOutputStream, outNbVertices, transform);
    }
}

void MeshFileWriter::operator<<(const aiScene* parMeshData)
{
    static_assert(sizeof(ai_real) == 4, "la taille des reels assimp est != 4");
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

    // ------------------------------------------------------------
    std::vector<HierarchyNode> hierarchy;

    if (parMeshData->mRootNode->mNumChildren == 1)
    {
        ReadHierarchy(parMeshData->mRootNode->mChildren[0], -1, hierarchy);
    }
    else
    {
        ReadHierarchy(parMeshData->mRootNode, -1, hierarchy);
    }
    fileHeader.NbNodesInHierarchy = (u32)hierarchy.size();
    fileHeader.layout.HasBones = true;
    // ------------------------------------------------------------

    VertexLayoutHash layoutHash(fileHeader.layout);
    fileHeader.VertexSizeInOctet = layoutHash.GetByteSize();

    FOutputStream.write((c8*)&fileHeader, sizeof(MeshFileHeader));

    u32 currentNodeIdx = 0;
    u32 nbVerticesWritten = 0;
    if (parMeshData->mRootNode->mNumChildren == 1)
    {
        ReadHierarchyMesh(parMeshData, parMeshData->mRootNode->mChildren[0], fileHeader, hierarchy, currentNodeIdx, FOutputStream, nbVerticesWritten);
    }
    else
    {
        ReadHierarchyMesh(parMeshData, parMeshData->mRootNode, fileHeader, hierarchy, currentNodeIdx, FOutputStream, nbVerticesWritten);
    }

    AssertRelease(nbVerticesWritten == fileHeader.NbVertices);
    AssertRelease(currentNodeIdx == (hierarchy.size() - 1));

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

    foreachitemconst(node, hierarchy)
    {
        FOutputStream.write((c8*)&node.Idx, sizeof(uc8));
        FOutputStream.write((c8*)&node.Parent, sizeof(uc8));
        const u32 nameLength = node.Name.size();
        FOutputStream.write((c8*)&nameLength, sizeof(u32));
        FOutputStream.write((c8*)node.Name.c_str(), nameLength);
        FOutputStream.write((c8*)&node.InverseLocalTransform, sizeof(glm::mat4));
    }
}

} // namespace Rendering
} // namespace ECSEngine
