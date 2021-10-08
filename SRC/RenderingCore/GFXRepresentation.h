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

class GFXRepresentation;
class GFXRepresentationDescriptor
{
    DECLARE_POOL_ALLOCATED(GFXRepresentationDescriptor);

public:
    const std::string& Name() const { return FName; }
    const std::string& MaterialName() const { return FMaterialName; }
    const std::string& MeshFile() const { return FMeshFile; }
    const MemoryView<const std::unique_ptr<AbstractGFXOperatorDescriptor>> OperatorDescriptors() const
    {
        return MemoryView(FOperatorDescriptors.data(), (u32)FOperatorDescriptors.size());
    }

    void DrawInEditor();

    SERIALIZE()
    {
        PROPERTYFIELD(Name, "Default");
        PROPERTYFIELD(MeshFile, "none");
        PROPERTYFIELD(MaterialName, "none");
        PROPERTYFIELD(OperatorDescriptors, std::vector<std::unique_ptr<AbstractGFXOperatorDescriptor>>());
    }

private:
    std::string FName = "Default";
    std::string FMeshFile = "none";
    std::string FMaterialName = "none";
    std::vector<std::unique_ptr<AbstractGFXOperatorDescriptor>> FOperatorDescriptors;
};

struct GFXRepresentationInitialiser
{
    float FCurrentTime = 0.f;

    glm::vec3 FPosition = glm::vec3(0.f);
    glm::quat FOrientation = glm::quat(1.f, 0.f, 0.f, 0.f);

    std::string FRepresentationDescriptor;

    std::pair<bool, bool> FIsSelectable;

    bool HasCarier = false;
    bool HasVisuals = false;
};

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
