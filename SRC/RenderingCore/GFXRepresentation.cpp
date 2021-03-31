#include "stdafx.h"

#include "GFXRepresentation.h"

#include "Application/PropertyDrawer.h"
#include "Carrier.h"
#include "GFXKeyHelper.h"
#include "VisualModel.h"

namespace ECSEngine
{
namespace Rendering
{
IMPLEMENT_POOL_ALLOCATED(GFXRepresentationDescriptor);

const GFXRepresentation* GFXRepresentationDescriptor::CreateRepresentation() const
{
    return new GFXRepresentation();
}

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

    forrange(i, 0, FOperatorDescriptors.size())
    {
        if (ImGui::CollapsingHeader(FOperatorDescriptors[i]->Name()))
        {
            FOperatorDescriptors[i]->DrawInEditor();
        }
    }
}

IMPLEMENT_POOL_ALLOCATED(GFXRepresentation);
GFXRepresentation::GFXRepresentation()
{
}

GFXRepresentation::~GFXRepresentation()
{
    FCarrier.reset(nullptr);
    FVisualModel.reset(nullptr);
}

void GFXRepresentation::Initialise(const GFXRepresentationInitialiser& parInit)
{
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
        FVisualModel->Init(parInit.FMaterialFilename, parInit.FMeshFileName);
    }
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
}

} // namespace Rendering
} // namespace ECSEngine
