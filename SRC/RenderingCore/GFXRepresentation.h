#pragma once
#include "Common/PoolAllocator.h"
#include "GFXMessage.h"

namespace ECSEngine
{
namespace Rendering
{
class Carrier;
class VisualModel;

struct GFXRepresentationInitialiser
{
    float FCurrentTime = 0.f;

    glm::vec3 FPosition = glm::vec3(0.f);
    glm::quat FOrientation = glm::quat(1.f, 0.f, 0.f, 0.f);

    std::string FMeshFileName = "";
    std::string FMaterialFilename = "";

    bool HasCarier = false;
    bool HasVisuals = false;
};

class GFXRepresentation
{
    DECLARE_POOL_ALLOCATED(GFXRepresentation);

public:
    GFXRepresentation();
    ~GFXRepresentation();

    void Initialise(const GFXRepresentationInitialiser& parInit);
    void Update(float parCurrentTime);
    void SwapQueues();

    GFXMessage& GetCurrentQueueForPushingMessage() { return FMessages[1 - FCurrentMessageQueue]; }

private:
    Carrier* FCarrier = nullptr;
    VisualModel* FVisualModel = nullptr;

    GFXMessage FMessages[2];
    u32 FCurrentMessageQueue = 0;
};
} // namespace Rendering
} // namespace ECSEngine