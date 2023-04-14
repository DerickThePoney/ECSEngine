#include "stdafx.h"

#include "RenderPass.h"

namespace ECSEngine
{
namespace Rendering
{
namespace RenderPassId
{
const char* RenderPassId::GetNameFromType(Type parPass)
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

Type GetTypeFromName(const std::string& parName)
{
    // clang-format off
    static const std::map<std::string, Type> NameToTypeMap{
#define NAME_TO_TYPE(TYPE) { std::string(#TYPE), TYPE }
        NAME_TO_TYPE(GEOMETRY_PASS),
        NAME_TO_TYPE(SELECTION_PASS),
        NAME_TO_TYPE(SELECTION_BLIT_PASS),
        NAME_TO_TYPE(FEEDBACK_PASS),
        NAME_TO_TYPE(OUTLINE_INIT),
        NAME_TO_TYPE(OUTLINE_SOLID),
        NAME_TO_TYPE(OUTLINE_HORIZONTAL),
        NAME_TO_TYPE(OUTLINE_VERTICAL),
        NAME_TO_TYPE(GAME_RENDERER_COMBINE_PASS),
        NAME_TO_TYPE(GAME_UI_PASS),
        NAME_TO_TYPE(FINAL_COMBINE_PASS),
        NAME_TO_TYPE(EDITOR_PASS),
        NAME_TO_TYPE(EDITOR_UI_PASS),
        NAME_TO_TYPE(DEBUG_PASS),
        NAME_TO_TYPE(IMGUI_EDITOR_PASS),
        NAME_TO_TYPE(IMGUI_UI_PASS),
        NAME_TO_TYPE(IMGUI_DEBUG_PASS)
    };
    // clang-format on

    auto itFind = NameToTypeMap.find(parName);
    if (itFind == NameToTypeMap.end())
    {
        AssertNotReached();
        return Type::END_OF_GAME_PASSES;
    }
    return itFind->second;
}

Type ChooseInList(Type parPreviouslyChosen)
{
    Type res = parPreviouslyChosen;
    if (ImGui::BeginCombo("##Render pass", GetNameFromType(parPreviouslyChosen)))
    {
        forrange(i, 0, (u32)END_OF_GAME_PASSES)
        {
            ImGui::PushID(i);
            Type current = (Type)i;
            bool isSelected = (current == parPreviouslyChosen);
            if (ImGui::Selectable(GetNameFromType(current), isSelected))
                res = current;

            if (isSelected)
                ImGui::SetItemDefaultFocus();
            ImGui::PopID();
        }
        ImGui::Separator();
        {
            bool isSelected = (EDITOR_PASS == parPreviouslyChosen);
            if (ImGui::Selectable(GetNameFromType(EDITOR_PASS), isSelected))
                res = EDITOR_PASS;

            if (isSelected)
                ImGui::SetItemDefaultFocus();

            isSelected = (IMGUI_EDITOR_PASS == parPreviouslyChosen);
            if (ImGui::Selectable(GetNameFromType(IMGUI_EDITOR_PASS), isSelected))
                res = IMGUI_EDITOR_PASS;

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::Separator();
        {
            bool isSelected = (DEBUG_PASS == parPreviouslyChosen);
            if (ImGui::Selectable(GetNameFromType(DEBUG_PASS), isSelected))
                res = DEBUG_PASS;

            if (isSelected)
                ImGui::SetItemDefaultFocus();

            isSelected = (IMGUI_DEBUG_PASS == parPreviouslyChosen);
            if (ImGui::Selectable(GetNameFromType(IMGUI_DEBUG_PASS), isSelected))
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
