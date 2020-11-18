#include "stdafx.h"

#include "RenderPass.h"

const char* ECSEngine::Rendering::RenderPassId::GetName(Type parPass)
{
#define CASE_ENUM_TO_CHAR(NAME)                                                                                                                                                    \
    case NAME:                                                                                                                                                                     \
        return #NAME
    switch (parPass)
    {
        CASE_ENUM_TO_CHAR(GEOMETRY_PASS);
        CASE_ENUM_TO_CHAR(SELECTION_PASS);
        CASE_ENUM_TO_CHAR(SELECTION_BLIT_PASS);
        CASE_ENUM_TO_CHAR(DEBUG_PASS);
        CASE_ENUM_TO_CHAR(IMGUI_EDITOR_PASS);
        CASE_ENUM_TO_CHAR(IMGUI_UI_PASS);
        CASE_ENUM_TO_CHAR(IMGUI_DEBUG_PASS);
    default:
        AssertNotReached();
        return "UNKNOWN PASS";
    }
}
