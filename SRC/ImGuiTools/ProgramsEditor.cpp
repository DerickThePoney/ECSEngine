#include "stdafx.h"

#include "ProgramsEditor.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceFile.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"

namespace ECSEngine
{
namespace ImGUITools
{
namespace
{
void DrawMenuBar()
{
}
}

void DrawProgramsEditor(bool& parIsOpen, float parMenuBarHeight)
{
    // list programs
    std::vector<std::string> programs;
    GlobalResourceCache::Instance().FCache->GetFileSystem()->ListResourceFiles("*.programV2", programs);

    std::vector<std::string> shaders;
    GlobalResourceCache::Instance().FCache->GetFileSystem()->ListResourceFiles("*.bin", shaders);

    // loop through all the programs and edit them
    glm::vec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    ImGui::SetNextWindowSize(windowSize * glm::vec2(1.0f, 1.0f - (parMenuBarHeight / windowSize.y)));
    ImGui::SetNextWindowPos(glm::vec2(0.0f, parMenuBarHeight));
    ImGui::Begin("Programs editor", &parIsOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);
    DrawMenuBar();

    // TODO :
    // - Category for programs lists (en gros un std::vector par fichier programV2
    // - pour chaque category, lister les programs dedans
    // - pour chaque program, editer le programs
    // donc en gros deux child window, avec une treeview d'un coté, une view de l'autre.

    ImGui::End();
}
} // namespace ImGUITools
} // namespace ECSEngine