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
        CASE_ENUM_TO_CHAR(GAME_UI_PASS);
        CASE_ENUM_TO_CHAR(COMBINE_PASS);
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

Type ChooseInList(RenderPassId::Type parPreviouslyChosen)
{
    if (ImGui::BeginCombo("Render pass", GetName(parPreviouslyChosen))) { }
    return parPreviouslyChosen;
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
