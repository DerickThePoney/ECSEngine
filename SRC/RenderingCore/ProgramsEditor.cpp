#include "stdafx.h"

#include "ProgramsEditor.h"

#include "Common/ResourceCache.h"
#include "Common/ResourceFile.h"
#include "GLFWDisplayWindowHandler.h"
#include "MaterialManager.h"
#include "MaterialDescriptors.h"

namespace ECSEngine
{
namespace Rendering
{

namespace ImGUITools
{

namespace
{
void SavePrograms()
{
    std::vector<ProgramDescriptorV2*>& programs = Rendering::MaterialManager::GetProgramsForEditor();
    foreachitemconst(program, programs)
    {
        std::ofstream ofstr(GlobalResourceCache::Instance().FCache->GetFileSystem()->GetBasePathName() + "\\" + program->Filename());
        AssertRelease(ofstr.good());
        cereal::JSONOutputArchive ar(ofstr);
        ar ( *program);
    }
}

void DrawMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save"))
            {
                SavePrograms();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}
} // namespace

void DrawProgramsEditor(bool& parIsOpen, float parMenuBarHeight)
{
    // list programs
    std::vector<std::string> shaders;
    GlobalResourceCache::Instance().FCache->GetFileSystem()->ListResourceFiles("*.bin", shaders);

    // loop through all the programs and edit them
    glm::vec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    ImGui::SetNextWindowSize(windowSize * glm::vec2(1.0f, 1.0f - (parMenuBarHeight / windowSize.y)));
    ImGui::SetNextWindowPos(glm::vec2(0.0f, parMenuBarHeight));
    ImGui::Begin("Programs editor", &parIsOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);
    DrawMenuBar();

    // TODO :
    // - Just list programs it's easier
    // - pour chaque category, lister les programs dedans
    // - pour chaque program, editer le programs
    // donc en gros deux child window, avec une treeview d'un coté, une view de l'autre.

    // PROBLEM, NEED TO SEPARATE THE DESCRIPTORS FROM THE INSTANTIATION....
    std::vector<ProgramDescriptorV2*>& programs = Rendering::MaterialManager::GetProgramsForEditor();

    if (ImGui::Button("Add Program"))
    {
        programs.push_back(new ProgramDescriptorV2());
    }

    u32 toDelete = -1;
    forrange(i, 0, programs.size())
    {
        ImGui::PushID(i);
        if (ImGui::Button("X"))
        {
            toDelete = i;
        }
        ImGui::SameLine();
        
        if (ImGui::CollapsingHeader(programs[i]->Filename().c_str()))
        {
            programs[i]->DrawEditor();
        }
        
        ImGui::PopID();
    }

    ImGui::End();
}
} // namespace ImGUITools
} // namespace Rendering
} // namespace ECSEngine