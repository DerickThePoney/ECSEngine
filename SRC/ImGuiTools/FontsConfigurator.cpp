#include "stdafx.h"

#include "FontsConfigurator.h"

#include "Application/PropertyDrawer.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "UICore/FontManager.h"

namespace ECSEngine
{
namespace ImGUITools
{

void FontsConfigMenu()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save"))
            {
                UI::Fonts::SaveFontFamiles();
            }

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

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

    FontsConfigMenu();

    if (ImGui::Button("Add font family"))
        ImGui::OpenPopup("New font family");

    if (ImGui::BeginPopupModal("New font family", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static std::string fontResource = "";
        EDITOR_PROPERTY_STRING("Chosen font", fontResource, true, "*.ttf");

        if (fontFamilies.find(fontResource) != fontFamilies.end())
        {
            ImGui::TextColored(glm::vec4(1.f, 1.f, 0.f, 200.f), "The chose font is already in the list of fonts. Only the size may be added if it is not already");
        }

        static int newSize = 1;
        ImGui::InputInt("Size to add", &newSize);
        if (newSize <= 0)
            newSize = 1;

        if (fontResource != "" && ImGui::Button("Add"))
        {
            fontFamilies[fontResource].insert(newSize);
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    foreachitem(fontFamily, fontFamilies)
    {
        ImGui::PushID(&fontFamily);
        if (ImGui::CollapsingHeader(fontFamily.first.c_str()))
        {
            std::ostringstream sstr;
            auto it = fontFamily.second.begin();
            sstr << *it;
            ++it;

            while (it != fontFamily.second.end())
            {
                sstr << "    -    " << *it;
                ++it;
            }

            ImGui::TextWrapped(sstr.str().c_str());

            if (ImGui::Button("Add Size"))
                ImGui::OpenPopup("New size");

            // New size code
            if (ImGui::BeginPopupModal("New size", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                static int newSize = 1;
                ImGui::InputInt("New size to add", &newSize);
                if (newSize <= 0)
                    newSize = 1;

                if (fontFamily.second.find(newSize) == fontFamily.second.end())
                {
                    if (ImGui::Button("Add"))
                    {
                        fontFamily.second.insert(newSize);
                        ImGui::CloseCurrentPopup();
                    }
                }
                else
                {
                    ImGui::TextColored(glm::vec4(1.f, 0.f, 0.f, 200.f), "Size %d is already in the set", newSize);
                }

                if (ImGui::Button("Cancel"))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
        ImGui::PopID();
    }
    ImGui::End();
}

} // namespace ImGUITools
} // namespace ECSEngine