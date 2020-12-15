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

    std::mutex FMutex;
};
} // namespace ECSEngine