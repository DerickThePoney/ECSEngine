#include "stdafx.h"

#include "ShaderType.h"

namespace ECSEngine
{
namespace Rendering
{
namespace ShaderType
{
const char* GetName(Type parPass)
{
#define CASE_ENUM_TO_CHAR(NAME)                                                                                                                                                    \
    case NAME:                                                                                                                                                                     \
        return #NAME
    switch (parPass)
    {
        CASE_ENUM_TO_CHAR(VERTEX_SHADER);
        CASE_ENUM_TO_CHAR(FRAGMENT_SHADER);
    default:
        AssertNotReached();
        return "UNKNOWN SHADER";
    }
}

Type ChooseInList(Type parPreviouslyChosen)
{
    Type res = parPreviouslyChosen;
    if (ImGui::BeginCombo("Shader Type", GetName(parPreviouslyChosen)))
    {
        forrange(i, 0, (u32)LENGTH)
        {
            ImGui::PushID(i);
            Type current = (Type)i;
            bool isSelected = (current == parPreviouslyChosen);
            if (ImGui::Selectable(GetName(current)), isSelected)
                res = current;

            if (isSelected)
                ImGui::SetItemDefaultFocus();
            ImGui::PopID();
        }
    }
    return res;
}
} // namespace ShaderType
} // namespace Rendering
} // namespace ECSEngine