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
}

void GameplayFeedbackDrawer::Shutdown()
{
}

void GameplayFeedbackDrawer::AddCircle(const Rendering::CircleFeedbackParameters& parCircleParameters, const glm::mat4& parTransform)
{
    std::scoped_lock<std::mutex> lock(FMutex);
    FCircles.push_back({ parCircleParameters, parTransform });
}

void GameplayFeedbackDrawer::AddGridChunk(const float parInnerCircleRadius, const float parOuterCircleRadius, float parThickness, const u32 parColor)
{
    std::scoped_lock<std::mutex> lock(FMutex);
    FGridChunks.push_back({ parInnerCircleRadius, parOuterCircleRadius, parThickness, parColor });
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
    foreachitemconst(gridChunk, FGridChunks)
    {
        parCommandBuffer->DrawCircularChunk(gridChunk.InnerCircleRadius, gridChunk.OuterCircleRadius, gridChunk.Thickness, gridChunk.Color, FGridChunkMaterial);
    }
    FGridChunks.clear();
}

} // namespace ECSEngine