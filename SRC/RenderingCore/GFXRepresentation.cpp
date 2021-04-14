#include "stdafx.h"

#include "GFXRepresentation.h"

#include "Application/PropertyDrawer.h"
#include "Carrier.h"
#include "GFXKeyHelper.h"
#include "GFXRepresentationDescriptorManager.h"
#include "SkelettonManager.h"
#include "VisualModel.h"

namespace ECSEngine
{
namespace Rendering
{
IMPLEMENT_POOL_ALLOCATED(GFXRepresentationDescriptor);

void GFXRepresentationDescriptor::DrawInEditor()
{
    EDITOR_PROPERTY_STRING("GFXRepresentation name", FName, false, "");
    EDITOR_PROPERTY_STRING("Mesh file name", FMeshFile, true, "*.fbx.gen");
    EDITOR_PROPERTY_STRING("Material file name", FMaterialName, true, "*.material");

    auto operatorsList = GFXOperatorDescriptorFactory::GetOperatorsList();
    static int selected = -1;
    if (ImGui::BeginCombo("##GFXOperatorList", (selected == -1) ? "" : operatorsList[selected]))
    {
        forrange(i, 0, operatorsList.size())
        {
            if (ImGui::Selectable(operatorsList[i], selected == (u32)i))
            {
                selected = (u32)i;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    if (ImGui::Button("Add GFX Operator"))
    {
        FOperatorDescriptors.push_back(std::unique_ptr<AbstractGFXOperatorDescriptor>(GFXOperatorDescriptorFactory::CreateOperator(operatorsList[selected])));
    }

    forrange(i, 0, FOperatorDescriptors.size()) { FOperatorDescriptors[i]->DrawInEditor(); }
}

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
        FVisualModel->Init(descriptor->MaterialName(), descriptor->MeshFile());

        FSkelettonPose = SkelettonManager::Instance().CreateSkelettonPose_ReturnPose(FId, FVisualModel->GetMeshHandle());
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
        FMessages[FCurrentMessageQueue].ClearMessages();
    }

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
