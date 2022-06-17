#include "stdafx.h"

#include "GFXRepresentationDescriptor.h"

namespace ECSEngine
{
namespace Rendering
{
IMPLEMENT_POOL_ALLOCATED(GFXRepresentationDescriptor);

void GFXRepresentationDescriptor::DrawInEditor()
{
    EDITOR_PROPERTY_STRING("GFXRepresentation name", FName, false, "");
    EDITOR_PROPERTY_STRING("Mesh file name", FMeshFile, true, "*.fbx.gen");
    EDITOR_PROPERTY_STRING("Material file name", FMaterialName, true, "*.material*");
    EDITOR_PROPERTY_BOOL("Is multipass material", FIsMultpassMaterial);

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
} // namespace Rendering
} // namespace ECSEngine