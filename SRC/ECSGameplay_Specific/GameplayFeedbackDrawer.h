#pragma once
#include "Common/IFeedbackDrawer.h"
#include "Common/PoolAllocator.h"
#include "Common/RenderingHandles.h"
#include "Common/Singleton.h"
#include "RenderingCore/FeedbackParameters.h"

namespace ECSEngine
{
namespace Rendering
{
struct CircleFeedbackParameters;
}
class GameplayFeedbackDrawer : public IFeedbackDrawer, public Singleton<GameplayFeedbackDrawer>
{
    DECLARE_POOL_ALLOCATED(GameplayFeedbackDrawer);

public:
    void Initialise();
    void Shutdown();
    void AddCircle(const Rendering::CircleFeedbackParameters& parCircleParameters, const glm::mat4& parTransform);
    void AddGridChunk(const float parInnerCircleRadius, const float parOuterCircleRadius, float parThickness, const u32 parColor);

protected:
    virtual void VirtualDrawFeedback(Rendering::DrawCommandBuffer* parCommandBuffer) override;

private:
    struct Circle
    {
        Rendering::CircleFeedbackParameters Parameters;
        glm::mat4 FTransfrom;
    };

    std::vector<Circle> FCircles;
    Rendering::MaterialInstanceHandle FCircleMaterial;

    struct GridChunk
    {
        float InnerCircleRadius;
        float OuterCircleRadius;
        float Thickness;
        u32 Color;
    };
    std::vector<GridChunk> FGridChunks;
    Rendering::MaterialInstanceHandle FGridChunkMaterial;

    std::mutex FMutex;
};
} // namespace ECSEngine