#include "stdafx.h"

#include "MeshMaterialApplicationUpdater.h"

#include "Common/Logger.h"
#include "Common/MeshStreamingData.h"
#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceFile.h"
#include "Common/ResourceHandle.h"
#include "Common/RingBuffer.h"
#include "Common/TimeManager.h"
#include "Common/Timer.h"
#include "Tools/AssimpWrapper/AssimpMeshDataLoading.h"
#include "assimp/scene.h"

namespace ECSEngine
{
struct PosColorVertex
{
    float x;
    float y;
    float z;
    uint32_t abgr;
};

static PosColorVertex cubeVertices[] = {
    { -1.0f, 1.0f, 1.0f, 0xff000000 },
    { 1.0f, 1.0f, 1.0f, 0xff0000ff },
    { -1.0f, -1.0f, 1.0f, 0xff00ff00 },
    { 1.0f, -1.0f, 1.0f, 0xff00ffff },
    { -1.0f, 1.0f, -1.0f, 0xffff0000 },
    { 1.0f, 1.0f, -1.0f, 0xffff00ff },
    { -1.0f, -1.0f, -1.0f, 0xffffff00 },
    { 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const u32 cubeTriList[] = {
    0,
    1,
    2,
    1,
    3,
    2,
    4,
    6,
    5,
    5,
    6,
    7,
    0,
    2,
    4,
    4,
    2,
    6,
    1,
    5,
    3,
    5,
    7,
    3,
    0,
    4,
    1,
    4,
    5,
    1,
    2,
    3,
    6,
    6,
    3,
    7,
};

MeshMaterialApplicationUpdater::MeshMaterialApplicationUpdater()
    : FShouldClose(false)
{
}

void MeshMaterialApplicationUpdater::Initialise()
{
    GlobalResourceCache::Instance().FCache->GetFileSystem()->ListResourceFiles("*.fbx", FMeshFiles);
}

void MeshMaterialApplicationUpdater::Shutdown()
{
}

bool MeshMaterialApplicationUpdater::CheckShouldFinish()
{
    return FShouldClose;
}

void MeshMaterialApplicationUpdater::StartUpdate()
{
}

void MeshMaterialApplicationUpdater::Update()
{

    static size_t i = 0;

    if (i < FMeshFiles.size())
    {
        const std::string& meshFile = FMeshFiles[i++];
        LOG_WARNING(meshFile);

        Resource meshResource(meshFile);

        std::shared_ptr<ResourceHandle> meshResourceHandle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&meshResource);

        AssimpLoading::GenerateMesh(GlobalResourceCache::Instance().FCache->GetBasePath() + "\\" + meshFile, meshResourceHandle->Buffer(), meshResourceHandle->Size());

        std::ifstream ifstr(GlobalResourceCache::Instance().FCache->GetBasePath() + "\\" + meshFile + ".gen", std::ifstream::binary);
        AssertRelease(ifstr.good());
        Rendering::MeshFileHeader fileHeader;
        ifstr.read((c8*)&fileHeader, sizeof(fileHeader));

        Rendering::VertexLayoutHash hash(fileHeader);

        std::vector<glm::vec3> vertices;
        std::vector<std::vector<u32>> colors;
        std::vector<u32> indices;
        vertices.resize(fileHeader.NbVertices);
        colors.resize(fileHeader.NbColorChannels);
        forrange(i, 0, fileHeader.NbColorChannels) colors[i].resize(fileHeader.NbVertices);
        indices.resize(fileHeader.NbIndices);

        forrange(i, 0, fileHeader.NbVertices)
        {
            ifstr.read((c8*)&vertices[i].x, 4);
            ifstr.read((c8*)&vertices[i].y, 4);
            ifstr.read((c8*)&vertices[i].z, 4);
            forrange(j, 0, fileHeader.NbColorChannels) { ifstr.read((c8*)&colors[j][i], 4); }
        }

        ifstr.read((c8*)indices.data(), 4u * fileHeader.NbIndices);
    }
    else
    {
        FShouldClose = true;
    }
}

void MeshMaterialApplicationUpdater::Render()
{
}

void MeshMaterialApplicationUpdater::EndUpdate()
{
}

MeshMaterialApplicationUpdaterWrapper::MeshMaterialApplicationUpdaterWrapper()
    : FWrappedGameplayUpdater(nullptr)
{
}

MeshMaterialApplicationUpdaterWrapper::~MeshMaterialApplicationUpdaterWrapper()
{
    AssertRelease(FWrappedGameplayUpdater == nullptr);
}

void MeshMaterialApplicationUpdaterWrapper::Initialise()
{
    AssertRelease(FWrappedGameplayUpdater == nullptr);
    FWrappedGameplayUpdater = new MeshMaterialApplicationUpdater();

    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->Initialise();
}

void MeshMaterialApplicationUpdaterWrapper::Shutdown()
{
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->Shutdown();

    delete FWrappedGameplayUpdater;
    FWrappedGameplayUpdater = nullptr;
}

bool MeshMaterialApplicationUpdaterWrapper::CheckShouldFinish()
{
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    return FWrappedGameplayUpdater->CheckShouldFinish();
}

void MeshMaterialApplicationUpdaterWrapper::StartUpdate()
{
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->StartUpdate();
}

void MeshMaterialApplicationUpdaterWrapper::Update()
{
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->Update();
}

void MeshMaterialApplicationUpdaterWrapper::Render()
{
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->Render();
}

void MeshMaterialApplicationUpdaterWrapper::EndUpdate()
{
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->EndUpdate();
}

} // namespace ECSEngine

CEREAL_REGISTER_TYPE(ECSEngine::MeshMaterialApplicationUpdaterWrapper);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::IGameplayUpdater, ECSEngine::MeshMaterialApplicationUpdaterWrapper);