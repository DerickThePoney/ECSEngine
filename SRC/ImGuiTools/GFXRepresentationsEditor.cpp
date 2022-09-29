#include "stdafx.h"

#include "GFXRepresentationsEditor.h"

#include "Common/ResourceCache.h"
#include "RenderingCore/GFXRepresentation.h"
#include "RenderingCore/GFXRepresentationDescriptor.h"
#include "RenderingCore/GFXRepresentationDescriptorManager.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"

#include <fstream>

namespace ECSEngine
{
namespace ImGUITools
{

void DrawEditorMenu()
{
    if (ImGui::BeginMenuBar())
    {

        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Add descriptor", "Ctrl+N"))
            {
                Rendering::GFXRepresentationDescriptorManager::Instance().AddDescriptor();
            }

            ImGui::Separator();
            if (ImGui::MenuItem("Save", "Ctrl+S"))
            {
                std::ofstream ofstr(GlobalResourceCache::Instance().FCache->GetBasePath() + "\\Configuration\\GFXRepresentationDescriptors.json");

                cereal::JSONOutputArchive archive(ofstr);

                archive(NAMEDPROPERTY("GFXRepresentationDescriptors", Rendering::GFXRepresentationDescriptorManager::Instance()));
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void DrawGFXRepresentationsEditor(bool* parOutOpen /*= nullptr*/, const float parMenuBarHeight /*= 0.0f*/)
{
    if (!*parOutOpen)
        return;
    AssertRelease(Rendering::GFXRepresentationDescriptorManager::HasInstance());

    uvec2 uwindowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    vec2 windowSize(uwindowSize.x, uwindowSize.y);
    ImGui::SetNextWindowSize(windowSize * vec2(1.0f, 1.0f - (parMenuBarHeight / windowSize.y)));
    ImGui::SetNextWindowPos(vec2(0.0f, parMenuBarHeight));

    ImGui::Begin("GFX representation templates", parOutOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);
    DrawEditorMenu();
    const MemoryView<std::unique_ptr<Rendering::GFXRepresentationDescriptor>> descriptors = Rendering::GFXRepresentationDescriptorManager::Instance().Decriptors();

    const vec2 currentWindowSize = ImGui::GetContentRegionAvail();
    const vec2 utilityPlace = currentWindowSize - 50.0f;
    const float listProportion = 0.30f;
    const vec2 listSize = utilityPlace * vec2(listProportion, 1.0f);
    ImGui::SetCursorPosX(((currentWindowSize - utilityPlace) * 0.5f).x);
    ImGui::BeginChild(ImGui::GetID("Entity templates list"), listSize, true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    static u32 selected = -1;
    forrange(i, 0, descriptors.size())
    {
        if (ImGui::Selectable(descriptors[i]->Name().c_str(), selected == i))
            selected = (u32)i;
    }
    ImGui::EndChild();

    ImGui::SameLine();

    Rendering::GFXRepresentationDescriptor* desc = (selected != -1) ? descriptors[selected].get() : nullptr;
    const vec2 editorPlace = utilityPlace * vec2(1 - listProportion, 1.0f);
    ImGui::BeginChild(ImGui::GetID("Entity template editor"), editorPlace, true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    if (desc != nullptr)
    {
        desc->DrawInEditor();
    }
    ImGui::EndChild();
    ImGui::End();
}

} // namespace ImGUITools
} // namespace ECSEngine
