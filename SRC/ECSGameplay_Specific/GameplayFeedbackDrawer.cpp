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
}

void GameplayFeedbackDrawer::Shutdown()
{
}

void GameplayFeedbackDrawer::AddCircle(const Rendering::CircleFeedbackParameters& parCircleParameters, const glm::mat4& parTransform)
{
    std::scoped_lock<std::mutex> lock(FMutex);
    FCircles.push_back({ parCircleParameters, parTransform });
}

void GameplayFeedbackDrawer::VirtualDrawFeedback(Rendering::DrawCommandBuffer* parCommandBuffer)
{
    std::scoped_lock<std::mutex> lock(FMutex);

    // circles
    AssertRelease(FCircleMaterial.IsValid());
    foreachitemconst(circle, FCircles) { parCommandBuffer->DrawCircle(FCircleMaterial, circle.Parameters, circle.FTransfrom); }
    FCircles.clear();
}

} // namespace ECSEngine