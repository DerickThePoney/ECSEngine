#pragma once
#include "Common/PoolAllocator.h"
#include "GFXMessage.h"
#include "GFXOperator.h"

namespace ECSEngine
{
namespace Rendering
{
class Carrier;
class VisualModel;
class GFXSelectable;

struct GFXRepresentationInitialiser;
class GFXRepresentation
{
    DECLARE_POOL_ALLOCATED(GFXRepresentation);

public:
    GFXRepresentation(const u32 parId);
    ~GFXRepresentation();

    void Initialise(const GFXRepresentationInitialiser& parInit);
    void Update(float parCurrentTime);
    void SwapQueues();

    GFXMessage& GetCurrentQueueForPushingMessage() { return FMessages[1 - FCurrentMessageQueue]; }

    const Carrier* GetCarrier() const { return FCarrier.get(); }
    const VisualModel* GetVisualModel() const { return FVisualModel.get(); }
    const SkelettonPose* GetPose() const { return FSkelettonPose; }
    const GFXSelectable* GetSelectable() const { return FSelectable.get(); }
    GFXSelectable* GetSelectable() { return FSelectable.get(); }

    u32 Id() const { return FId; }

private:
    void ProcessMessages();
    void UpdateOperators();

private:
    std::unique_ptr<Carrier> FCarrier = nullptr;
    std::unique_ptr<VisualModel> FVisualModel = nullptr;
    SkelettonPose* FSkelettonPose = nullptr;

    std::unique_ptr<GFXSelectable> FSelectable = nullptr;

    std::vector<std::unique_ptr<AbstractGFXOperator>> FGFXOperators;

    GFXMessage FMessages[2];
    u32 FCurrentMessageQueue = 0;

    const u32 FId = -1;
};
} // namespace Rendering
} // namespace ECSEngine
