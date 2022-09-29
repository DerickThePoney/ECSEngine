#pragma once
#include "Common/IFeedbackDrawer.h"
#include "Common/PoolAllocator.h"
#include "Common/RenderingHandles.h"
#include "Common/Singleton.h"
#include "RenderingCore/FeedbackParameters.h"
#include <mutex>

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
    void AddCircle(const Rendering::CircleFeedbackParameters& parCircleParameters, const mat4& parTransform);
    void AddAABB(const vec3& parMin, const vec3& parMax, const u32 parColor, const mat4& parTransform, bool parAsCubes);
    void AddGridChunk(const float parInnerCircleRadius, const float parOuterCircleRadius, const float parThickness, const float parArcAngle, const u32 parColor);

protected:
    virtual void VirtualDrawFeedback(Rendering::DrawCommandBuffer* parCommandBuffer) override;

private:
    struct Circle
    {
        Rendering::CircleFeedbackParameters Parameters;
        mat4 FTransfrom;
    };

    std::vector<Circle> FCircles;
    Rendering::MaterialInstanceHandle FCircleMaterial;

    std::vector<Rendering::CircularGridChunkFeedbackParameters> FGridChunks;
    Rendering::MaterialInstanceHandle FGridChunkMaterial;

    struct AABB
    {
        vec3 Min;
        vec3 Max;
        u32 Color;
        mat4 FTransfrom;
        bool AsCube;
    };
    std::vector<AABB> FAABB;

    Rendering::MaterialInstanceHandle FVertexColorMaterial;

    std::mutex FMutex;
};
} // namespace ECSEngine
