#include "stdafx.h"

#include "RenderPass.h"

namespace ECSEngine
{
namespace Rendering
{
namespace RenderPassId
{
const char* RenderPassId::GetName(Type parPass)
{
#define CASE_ENUM_TO_CHAR(NAME)                                                                                                                                                    \
    case NAME:                                                                                                                                                                     \
        return #NAME
    switch (parPass)
    {
        CASE_ENUM_TO_CHAR(GEOMETRY_PASS);
        CASE_ENUM_TO_CHAR(SELECTION_PASS);
        CASE_ENUM_TO_CHAR(SELECTION_BLIT_PASS);
        CASE_ENUM_TO_CHAR(FEEDBACK_PASS);
        CASE_ENUM_TO_CHAR(OUTLINE_INIT);
        CASE_ENUM_TO_CHAR(OUTLINE_SOLID);
        CASE_ENUM_TO_CHAR(OUTLINE_HORIZONTAL);
        CASE_ENUM_TO_CHAR(OUTLINE_VERTICAL);
        CASE_ENUM_TO_CHAR(GAME_RENDERER_COMBINE_PASS);
        CASE_ENUM_TO_CHAR(GAME_UI_PASS);
        CASE_ENUM_TO_CHAR(FINAL_COMBINE_PASS);
        CASE_ENUM_TO_CHAR(EDITOR_PASS);
        CASE_ENUM_TO_CHAR(EDITOR_UI_PASS);
        CASE_ENUM_TO_CHAR(DEBUG_PASS);
        CASE_ENUM_TO_CHAR(IMGUI_EDITOR_PASS);
        CASE_ENUM_TO_CHAR(IMGUI_UI_PASS);
        CASE_ENUM_TO_CHAR(IMGUI_DEBUG_PASS);
    default:
        AssertNotReached();
        return "UNKNOWN PASS";
    }
}

Type ChooseInList(Type parPreviouslyChosen)
{
    Type res = parPreviouslyChosen;
    if (ImGui::BeginCombo("##Render pass", GetName(parPreviouslyChosen)))
    {
        forrange(i, 0, (u32)END_OF_GAME_PASSES)
        {
            ImGui::PushID(i);
            Type current = (Type)i;
            bool isSelected = (current == parPreviouslyChosen);
            if (ImGui::Selectable(GetName(current), isSelected))
                res = current;

            if (isSelected)
                ImGui::SetItemDefaultFocus();
            ImGui::PopID();
        }
        ImGui::Separator();
        {
            bool isSelected = (EDITOR_PASS == parPreviouslyChosen);
            if (ImGui::Selectable(GetName(EDITOR_PASS), isSelected))
                res = EDITOR_PASS;

            if (isSelected)
                ImGui::SetItemDefaultFocus();

            isSelected = (IMGUI_EDITOR_PASS == parPreviouslyChosen);
            if (ImGui::Selectable(GetName(IMGUI_EDITOR_PASS), isSelected))
                res = IMGUI_EDITOR_PASS;

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::Separator();
        {
            bool isSelected = (DEBUG_PASS == parPreviouslyChosen);
            if (ImGui::Selectable(GetName(DEBUG_PASS), isSelected))
                res = DEBUG_PASS;

            if (isSelected)
                ImGui::SetItemDefaultFocus();

            isSelected = (IMGUI_DEBUG_PASS == parPreviouslyChosen);
            if (ImGui::Selectable(GetName(IMGUI_DEBUG_PASS), isSelected))
                res = IMGUI_DEBUG_PASS;

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    return res;
}
} // namespace RenderPassId

RenderPassDescriptor::RenderPassDescriptor()
{
}

RenderPassDescriptor::~RenderPassDescriptor()
{
}

} // namespace Rendering
} // namespace ECSEngine
