#include "stdafx.h"

#include "GameplayFeedbackDrawer.h"

#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/MaterialManager.h"

namespace ECSEngine
{
IMPLEMENT_POOL_ALLOCATED(GameplayFeedbackDrawer);

void GameplayFeedbackDrawer::Initialise()
{
    FCircleMaterial = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\feedbackmaterial.material");
    AssertRelease(FCircleMaterial.IsValid());

    FGridChunkMaterial = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\circulargridchunk.material");
    AssertRelease(FGridChunkMaterial.IsValid());

    FVertexColorMaterial = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\vertexcolormaterial.material");
    AssertRelease(FVertexColorMaterial.IsValid());
}

void GameplayFeedbackDrawer::Shutdown()
{
}

void GameplayFeedbackDrawer::AddCircle(const Rendering::CircleFeedbackParameters& parCircleParameters, const glm::mat4& parTransform)
{
    std::scoped_lock<std::mutex> lock(FMutex);
    FCircles.push_back({ parCircleParameters, parTransform });
}

void GameplayFeedbackDrawer::AddAABB(const glm::vec3& parMin, const glm::vec3& parMax, const u32 parColor, const glm::mat4& parTransform, bool parAsCubes)
{
    std::scoped_lock<std::mutex> lock(FMutex);
    FAABB.push_back({ parMin, parMax, parColor, parTransform, parAsCubes });
}

void GameplayFeedbackDrawer::AddGridChunk(const float parInnerCircleRadius, const float parOuterCircleRadius, const float parThickness, const float parArcAngle, const u32 parColor)
{
    std::scoped_lock<std::mutex> lock(FMutex);
    FGridChunks.push_back({ parInnerCircleRadius, parOuterCircleRadius, parThickness, parArcAngle, parColor });
}

void GameplayFeedbackDrawer::VirtualDrawFeedback(Rendering::DrawCommandBuffer* parCommandBuffer)
{
    std::scoped_lock<std::mutex> lock(FMutex);

    // circles
    AssertRelease(FCircleMaterial.IsValid());
    foreachitemconst(circle, FCircles) { parCommandBuffer->DrawCircle(FCircleMaterial, circle.Parameters, circle.FTransfrom); }
    FCircles.clear();

    // grid chunks
    AssertRelease(FGridChunkMaterial.IsValid());
    foreachitemconst(gridChunk, FGridChunks) { parCommandBuffer->DrawCircularChunk(gridChunk, FGridChunkMaterial); }
    FGridChunks.clear();

    // aabbs
    AssertRelease(FVertexColorMaterial.IsValid());
    foreachitemconst(aabb, FAABB)
    {
        if (aabb.AsCube)
            parCommandBuffer->DrawAABBAsCube(FVertexColorMaterial, aabb.Min, aabb.Max, aabb.Color);
        else
            parCommandBuffer->DrawAABB(FVertexColorMaterial, aabb.Min, aabb.Max, aabb.Color);
    }
    FAABB.clear();
}

} // namespace ECSEngine