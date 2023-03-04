#include "stdafx.h"

#include "MeshFileReader.h"

#include "Common/MeshStreamingData.h"
#include "Common/RenderingHandles.h"
#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
#include "Mesh.h"
#include "MeshUtils.h"
#include "SkelettonManager.h"
#include "VertexLayout.h"

#include <fstream>

namespace ECSEngine
{
namespace Rendering
{

namespace
{
void ReadSkelettonImplementation(const MeshHandle parHandle, const MeshFileHeader& parHeader, std::istream& parStream)
{
    if (parHeader.NbNodesInHierarchy == 1)
        return;

    std::vector<HierarchyNode> joints(parHeader.NbNodesInHierarchy);
    c8 buff[1024];
    forrange(i, 0, joints.size())
    {
        HierarchyNode& joint = joints[i];
        parStream.read((c8*)&joint.Idx, sizeof(uc8));
        parStream.read((c8*)&joint.Parent, sizeof(uc8));
        u32 nameLength = 0;
        parStream.read((c8*)&nameLength, sizeof(u32));
        AssertRelease(nameLength > 0);
        parStream.read((c8*)buff, nameLength);
        joint.Name = std::string(buff, nameLength);
        parStream.read((c8*)&joint.InverseLocalTransform, sizeof(mat4));
    }

    Skeletton* skeletton = SkelettonManager::Instance().CreateSkeletton_ReturnCreatedSkeletton(parHandle);
    skeletton->SetBonesNumber(joints.size());
    forrange(i, 0, joints.size())
    {
        HierarchyNode& node = joints[i];
        SkelettonJoint& joint = skeletton->GetJoint((u8)i);
        joint.InvBindPose = node.InverseLocalTransform;
        joint.ParentId = node.Parent;
        joint.Name = node.Name;
    }

    bool allGood = false;
    std::vector<bool> computed(joints.size(), false);
    while (!allGood)
    {
        forrange(i, 0, joints.size())
        {
            if (computed[i])
                continue;
            SkelettonJoint& joint = skeletton->GetJoint((u8)i);
            if (joint.ParentId == 0xFF)
            {
                joint.ModelToJointMatrix = mat4::Identity();
                computed[i] = true;
                continue;
            }

            if (computed[joint.ParentId])
            {
                SkelettonJoint& parent = skeletton->GetJoint(joint.ParentId);
                joint.ModelToJointMatrix = joint.InvBindPose * parent.ModelToJointMatrix;
                computed[i] = true;
                continue;
            }
        }

        allGood = true;
        forrange(i, 0, joints.size())
        {
            allGood = allGood && computed[i];
        }
    }
}

void ReadMeshImplementation(const MeshHandle parHandle, Mesh*& parMesh, std::istream& parStream)
{
    MeshFileHeader fileHeader;
    parStream.read((c8*)&fileHeader, sizeof(MeshFileHeader));

    AssertRelease(fileHeader.MajorVersion == MagicStuff::MajorVersion);
    AssertRelease(fileHeader.MinorVersion == MagicStuff::MinorVersion);

    VertexLayoutHash hash(fileHeader.layout);

    VertexDataStream vertexDataStream(fileHeader.NbVertices, fileHeader.VertexSizeInOctet, hash);

    std::vector<u32> indices;
    vec3 verticesGravityCenter = vec3(0.0f);
    indices.resize(fileHeader.NbIndices);
    forrange(i, 0, fileHeader.NbVertices)
    {
        if (fileHeader.layout.HasPositions)
        {
            vec3 data(0.0f);
            parStream.read((c8*)&data.x, 4);
            parStream.read((c8*)&data.y, 4);
            parStream.read((c8*)&data.z, 4);
            verticesGravityCenter += data;
            vertexDataStream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, data);
        }

        forrange(j, 0, fileHeader.layout.NbColorChannels)
        {
            u32 color = 0;
            parStream.read((c8*)&color, 4);
            vertexDataStream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, color);
        }

        forrange(j, 0, fileHeader.layout.NbUVs)
        {
            vec2 uv(0.0f);
            parStream.read((c8*)&uv.x, 4);
            parStream.read((c8*)&uv.y, 4);
            vertexDataStream.PushData(VERTEX_LAYOUT_PARAMS::HAS_UVS, 0, uv);
        }

        if (fileHeader.layout.HasNormals)
        {
            vec3 data(0.0f);
            parStream.read((c8*)&data.x, 4);
            parStream.read((c8*)&data.y, 4);
            parStream.read((c8*)&data.z, 4);
            vertexDataStream.PushData(VERTEX_LAYOUT_PARAMS::HAS_NORMALS, 0, data);
        }

        if (fileHeader.layout.HasTangents)
        {
            vec3 data(0.0f);
            parStream.read((c8*)&data.x, 4);
            parStream.read((c8*)&data.y, 4);
            parStream.read((c8*)&data.z, 4);
            vertexDataStream.PushData(VERTEX_LAYOUT_PARAMS::HAS_TANGENTS, 0, data);
        }

        if (fileHeader.layout.HasBinormals)
        {
            vec3 data(0.0f);
            parStream.read((c8*)&data.x, 4);
            parStream.read((c8*)&data.y, 4);
            parStream.read((c8*)&data.z, 4);
            vertexDataStream.PushData(VERTEX_LAYOUT_PARAMS::HAS_BINORMALS, 0, data);
        }

        if (fileHeader.layout.HasBones)
        {
            BlendIndices nodeIdx;
            parStream.read((c8*)&nodeIdx, sizeof(BlendIndices));
            AssertRelease(nodeIdx.Indices[0] < fileHeader.NbNodesInHierarchy);
            vertexDataStream.PushData(VERTEX_LAYOUT_PARAMS::HAS_BONES, 0, nodeIdx.pack);
        }

        vertexDataStream.Advance();
    }

    verticesGravityCenter /= fileHeader.NbVertices;
    float radius = 0.0f;
    if (fileHeader.layout.HasPositions)
    {
        forrange(i, 0, fileHeader.NbVertices)
        {
            const vec3 currentVertex = vertexDataStream.GetValue<vec3>(Rendering::VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, (u32)i);
            radius = std::max(radius, LengthSq(currentVertex - verticesGravityCenter));
        }
    }

    radius = std::sqrt(radius);

    parStream.read((c8*)indices.data(), 4u * fileHeader.NbIndices);

    parMesh = MeshHelpers::CreateIMesh();

    AssertRelease(parMesh != nullptr);

    parMesh->SetRawVertexData(vertexDataStream);
    parMesh->SetRawIndexData(indices.data(), fileHeader.NbIndices * 4u);
    parMesh->SetBoundingCircle(vec4(verticesGravityCenter, radius));

    ReadSkelettonImplementation(parHandle, fileHeader, parStream);
}
} // namespace

MeshFileReader::MeshFileReader()
{
}

MeshFileReader::~MeshFileReader()
{
}

void MeshFileReader::ReadMesh(const MeshHandle parHandle, Mesh*& parMesh, const std::string& parFilename)
{
    std::ifstream ifstr(parFilename.c_str(), std::ifstream::binary);
    AssertRelease(ifstr.good());
    ReadMeshImplementation(parHandle, parMesh, ifstr);
    parMesh->SetMeshFilename(parFilename);
}

void MeshFileReader::ReadMesh(const MeshHandle parHandle, Mesh*& parMesh, const Resource& parResource)
{
    std::shared_ptr<ResourceHandle> handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&parResource);

    ResourceBuffer buff = handle->GetResourceBuffer();
    std::istream sstr(&buff, std::istream::binary);
    ReadMeshImplementation(parHandle, parMesh, sstr);
    parMesh->SetMeshFilename(parResource.FName);
}

} // namespace Rendering
} // namespace ECSEngine
