#include "stdafx.h"

#include "MaterialEditor.h"

#include "Common/ResourceCache.h"
#include "Common/ResourceFile.h"
#include "GLFWDisplayWindowHandler.h"
#include "MaterialDescriptors.h"
#include "MaterialManager.h"

#include <fstream>

namespace ECSEngine
{
namespace Rendering
{
namespace ImGUITools
{
namespace
{
void SaveMaterials()
{
    std::vector<MultiPassMaterialDescriptor*>& materials = Rendering::MaterialManager::GetMaterialsForEditor();
    foreachitemconst(material, materials)
    {
        std::ofstream ofstr(GlobalResourceCache::Instance().FCache->GetFileSystem()->GetBasePathName() + "\\" + material->Filename());
        AssertRelease(ofstr.good());
        cereal::JSONOutputArchive ar(ofstr);
        ar(*material);
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
                SaveMaterials();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}
} // namespace

void DrawMaterialsEditor(bool& parIsOpen, float parMenuBarHeight)
{
    // list programs
    std::vector<std::string> shaders;
    GlobalResourceCache::Instance().FCache->GetFileSystem()->ListResourceFiles("*.bin", shaders);

    // loop through all the programs and edit them
    glm::vec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    ImGui::SetNextWindowSize(windowSize * glm::vec2(1.0f, 1.0f - (parMenuBarHeight / windowSize.y)));
    ImGui::SetNextWindowPos(glm::vec2(0.0f, parMenuBarHeight));
    ImGui::Begin("Materials editor", &parIsOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);
    DrawMenuBar();

    std::vector<MultiPassMaterialDescriptor*>& materials = Rendering::MaterialManager::GetMaterialsForEditor();

    if (ImGui::Button("Add Material"))
    {
        materials.push_back(new MultiPassMaterialDescriptor());
    }

    u32 toDelete = -1;
    forrange(i, 0, materials.size())
    {
        ImGui::PushID(i);
        if (ImGui::Button("X"))
        {
            toDelete = i;
        }
        ImGui::SameLine();

        if (ImGui::CollapsingHeader(materials[i]->Filename().c_str()))
        {
            materials[i]->DrawEditor();
        }

        ImGui::PopID();
    }

    if (toDelete != -1)
        materials.erase(materials.begin() + toDelete);

    ImGui::End();
}
} // namespace ImGUITools
} // namespace Rendering
} // namespace ECSEngine
