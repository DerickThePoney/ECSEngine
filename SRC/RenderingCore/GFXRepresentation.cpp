#include "stdafx.h"

#include "GFXRepresentation.h"

#include "Application/PropertyDrawer.h"
#include "Carrier.h"
#include "GFXKeyHelper.h"
#include "GFXRepresentationDescriptor.h"
#include "GFXRepresentationDescriptorManager.h"
#include "GFXRepresentationInitialiser.h"
#include "GFXSelectable.h"
#include "SkelettonManager.h"
#include "VisualModel.h"

namespace ECSEngine
{
namespace Rendering
{
IMPLEMENT_POOL_ALLOCATED(GFXRepresentation);
GFXRepresentation::GFXRepresentation(const u32 parId)
    : FId(parId)
{
}

GFXRepresentation::~GFXRepresentation()
{
    FCarrier.reset(nullptr);
    FVisualModel.reset(nullptr);
    FGFXOperators.clear();
    SkelettonManager::Instance().DeleteSkelettonPose(FId);
}

void GFXRepresentation::Initialise(const GFXRepresentationInitialiser& parInit)
{
    AssertRelease(!parInit.FRepresentationDescriptor.empty());
    const GFXRepresentationDescriptor* descriptor = GFXRepresentationDescriptorManager::Instance().Descriptor(parInit.FRepresentationDescriptor);
    AssertRelease(descriptor != nullptr);

    if (parInit.HasCarier)
    {
        FCarrier.reset(new Carrier());
        AssertRelease(FCarrier != nullptr);
        FCarrier->Init(parInit.FPosition, parInit.FOrientation, parInit.FCurrentTime);
    }

    if (parInit.HasVisuals)
    {
        FVisualModel.reset(new VisualModel());
        AssertRelease(FVisualModel != nullptr);
        FVisualModel->Init(descriptor->MaterialName(), descriptor->IsMultiPassMaterial(), descriptor->MeshFile());

        FSkelettonPose = SkelettonManager::Instance().CreateSkelettonPose_ReturnPose(FId, FVisualModel->GetMeshHandle());
    }

    if (parInit.FIsSelectable.first)
    {
        FSelectable.reset(new GFXSelectable());
        AssertRelease(FSelectable != nullptr);
        FSelectable->Init(parInit.FIsSelectable.second);
    }

    const MemoryView<const std::unique_ptr<AbstractGFXOperatorDescriptor>> operators = descriptor->OperatorDescriptors();
    FGFXOperators.reserve(operators.size());
    forrange(i, 0, operators.size()) { FGFXOperators.push_back(std::unique_ptr<AbstractGFXOperator>(operators[i]->CreateOperator())); }
}

void GFXRepresentation::Update(float parCurrentTime)
{
    if (FMessages[FCurrentMessageQueue].HasMessages())
    {
        ProcessMessages();
    }

    UpdateOperators();

    FMessages[FCurrentMessageQueue].ClearMessages();

    if (FCarrier != nullptr)
        FCarrier->Update(parCurrentTime);

    if (FVisualModel != nullptr)
    {
        /// DOSTUFF ON UPDATE IFN? ANIMATIONS UPDATE?
    }
}

void GFXRepresentation::SwapQueues()
{
    FCurrentMessageQueue = 1 - FCurrentMessageQueue;
}

void GFXRepresentation::ProcessMessages()
{
    GFXMessage& currentMessages = FMessages[FCurrentMessageQueue];

    if (FCarrier != nullptr)
    {
        auto newPositionKeyframe = currentMessages.GetValueIFP<glm::vec3>(GFXKeyHelper::Instance().Position);
        auto newOrientationKeyframe = currentMessages.GetValueIFP<glm::quat>(GFXKeyHelper::Instance().Orientation);

        if (newPositionKeyframe.second != -1 && newOrientationKeyframe.second != -1)
        {
            AlwaysCheckedAssert(newPositionKeyframe.second == newOrientationKeyframe.second);
            FCarrier->PushNewFullKeyframe(newPositionKeyframe.first, newOrientationKeyframe.first, newPositionKeyframe.second);
        }
        else if (newPositionKeyframe.second != -1)
        {
            FCarrier->PushNewPositionKeyframe(newPositionKeyframe.first, newPositionKeyframe.second);
        }
        else if (newOrientationKeyframe.second != -1)
        {
            FCarrier->PushNewRotationKeyframe(newOrientationKeyframe.first, newOrientationKeyframe.second);
        }
    }

    if (FVisualModel != nullptr && currentMessages.HasMessage(GFXKeyHelper::Instance().Visible))
    {
        auto visible = currentMessages.GetValueIFP<bool>(GFXKeyHelper::Instance().Visible);
        FVisualModel->SetVisible(visible.first);
    }

    if (FSelectable != nullptr && currentMessages.HasMessage(GFXKeyHelper::Instance().Selectable))
    {
        auto selectable = currentMessages.GetValueIFP<bool>(GFXKeyHelper::Instance().Selectable);
        FSelectable->SetSelectable(selectable.first);
    }
}

void GFXRepresentation::UpdateOperators()
{
    GFXMessage& currentMessages = FMessages[FCurrentMessageQueue];
    foreachitem(op, FGFXOperators)
    {
        if (op->GetMask() & OperatorMask::APPLY_ON_MESH)
        {
            op->ApplyChangesOnMesh(currentMessages, FVisualModel.get(), FSkelettonPose, FCarrier.get());
        }
    }
}

} // namespace Rendering
} // namespace ECSEngine
