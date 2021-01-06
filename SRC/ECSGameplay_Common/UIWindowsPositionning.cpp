#include "stdafx.h"

#include "UIWindowsPositionning.h"

namespace ECSEngine
{
namespace UI
{
namespace SIZE_TYPE
{

const char* GetName(Type parType)
{
    switch (parType)
    {
    case ECSEngine::UI::SIZE_TYPE::PIXELS:
        return "PIXELS\0";
    case ECSEngine::UI::SIZE_TYPE::PROPORTION:
        return "PROPORTION\0";
    }

    AssertNotReached();
    return nullptr;
}

} // namespace SIZE_TYPE


/*if (ImGui::BeginCombo("X SizeType", SIZE_TYPE::GetName(FSize.SizeXType)))
    {
        forrange(i, 0, SIZE_TYPE::LENGTH)
        {
            if (ImGui::Selectable(SIZE_TYPE::GetName((SIZE_TYPE::Type)i)))
            {
                FSize.SizeXType = (SIZE_TYPE::Type)i;
                break;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    ImGui::InputFloat("Size X", &FSize.SizeX);

    if (ImGui::BeginCombo("Y SizeType", SIZE_TYPE::GetName(FSize.SizeYType)))
    {
        forrange(i, 0, SIZE_TYPE::LENGTH)
        {
            if (ImGui::Selectable(SIZE_TYPE::GetName((SIZE_TYPE::Type)i)))
            {
                FSize.SizeYType = (SIZE_TYPE::Type)i;
                break;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    ImGui::InputFloat("Size Y", &FSize.SizeY);
    */

} // namespace UI
} // namespace ECSEngine