#include "stdafx.h"

#include "FontsConfigurator.h"

#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "UICore/FontManager.h"

namespace ECSEngine
{
namespace ImGUITools
{

void DrawFontsConfigurator(bool* parOpen, const float parMenuBarHeight /*= 0.f*/)
{
    if (!*parOpen)
        return;

    glm::vec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    ImGui::SetNextWindowSize(windowSize * glm::vec2(1.0f, 1.0f - (parMenuBarHeight / windowSize.y)));
    ImGui::SetNextWindowPos(glm::vec2(0.0f, parMenuBarHeight));

    UI::FontFamiliesSizes& fontFamilies = UI::Fonts::GetFontFamilies();
    AssertRelease(!fontFamilies.empty());

    ImGui::Begin("Fonts configurations", parOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);
    foreachitem(fontFamily, fontFamilies)
    {
        if (ImGui::CollapsingHeader(fontFamily.first.c_str()))
        {
            std::ostringstream sstr;
            auto it = fontFamily.second.begin();
            sstr << *it;
            ++it;

            while (it != fontFamily.second.end())
            {
                sstr << "    -    " << *it;
            }

            ImGui::TextWrapped(sstr.str().c_str());
        }
    }
    ImGui::End();
}

} // namespace ImGUITools
} // namespace ECSEngine