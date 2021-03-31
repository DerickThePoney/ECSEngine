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

class GFXRepresentation;
class GFXRepresentationDescriptor
{
    DECLARE_POOL_ALLOCATED(GFXRepresentationDescriptor);

public:
    const GFXRepresentation* CreateRepresentation() const;

    const std::string& Name() const { return FName; }
    const std::string& MaterialName() const { return FMaterialName; }
    const std::string& MeshFile() const { return FMeshFile; }
    const MemoryView<const std::unique_ptr<AbstractGFXOperatorDescriptor>> OperatorDescriptors() const
    {
        return MemoryView(FOperatorDescriptors.data(), (u32)FOperatorDescriptors.size());
    }

    void DrawInEditor();

protected:
    SERIALIZE()
    {
        PROPERTYFIELD(Name, "");
        PROPERTYFIELD(MeshFile, "");
        PROPERTYFIELD(MaterialName, "");
        PROPERTYFIELD(OperatorDescriptors, std::vector<std::unique_ptr<AbstractGFXOperatorDescriptor>>());
    }

private:
    std::string FName = "Default";
    std::string FMeshFile = "none";
    std::string FMaterialName = "none";
    std::vector<std::unique_ptr<AbstractGFXOperatorDescriptor>> FOperatorDescriptors;
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

    const Carrier* GetCarrier() const { return FCarrier.get(); }
    const VisualModel* GetVisualModel() const { return FVisualModel.get(); }

private:
    void ProcessMessages();

private:
    std::unique_ptr<Carrier> FCarrier = nullptr;
    std::unique_ptr<VisualModel> FVisualModel = nullptr;

    GFXMessage FMessages[2];
    u32 FCurrentMessageQueue = 0;
};
} // namespace Rendering
} // namespace ECSEngine
