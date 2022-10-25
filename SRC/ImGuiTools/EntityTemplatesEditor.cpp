#include "stdafx.h"

#include "EntityTemplatesEditor.h"

#include "../RenderingCore/GLFWDisplayWindowHandler.h"
#include "Common/ResourceCache.h"
#include "ECSCore/EntityTemplate.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleTemplate.h"
#include "ECSCore/WorldIds.h"

#include <fstream>

namespace ECSEngine
{
namespace ImGUITools
{
void DrawEditorMenu(EEntityWorlds parWorldFilter, int parSelected)
{
    if (ImGui::BeginMenuBar())
    {

        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Add template", "Ctrl+N"))
            {
                EntityTemplate* newTemplate = EntityTemplateManager::Instance().CreateNewEntityTemplate();
                newTemplate->SetWorldId_IKnowWhatImDoing(parWorldFilter);
            }

            if (ImGui::MenuItem("Add template and set modules as selected", "Ctrl+C", nullptr, parSelected != -1))
            {
                const EntityTemplate* etToClone = EntityTemplateManager::Instance().GetEntityTemplate(parSelected);
                EntityTemplate* newTemplate = EntityTemplateManager::Instance().CreateNewEntityTemplate();
                etToClone->CopyModuleTemplatesMapTo(newTemplate);
            }

            ImGui::Separator();
            if (ImGui::MenuItem("Save", "Ctrl+S"))
            {
                std::ofstream ofstr(GlobalResourceCache::Instance().FCache->GetBasePath() + "\\Configuration\\EntityTemplates.json");

                cereal::JSONOutputArchive archive(ofstr);

                archive(NAMEDPROPERTY("EntityTemplatesList", EntityTemplateManager::Instance()));
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void DrawEntityTemplatesEditor(bool* parOutOpen /*=nullptr*/, const float parMenuBarHeight /*= 0.0f*/)
{
    if (!*parOutOpen)
        return;
    AssertRelease(EntityTemplateManager::HasInstance());
    const u32 templatesToDraw = EntityTemplateManager::Instance().GetEntityTemplatesNumber();
    static EEntityWorlds worldIdFilter = EEntityWorlds::LENGTH;

    const uvec2 uwindowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    const vec2 windowSize(uwindowSize.x, uwindowSize.y);
    ImGui::SetNextWindowSize(windowSize * vec2(1.0f, 1.0f - (parMenuBarHeight / windowSize.y)));
    ImGui::SetNextWindowPos(vec2(0.0f, parMenuBarHeight));

    ImGui::Begin("Entity templates editor", parOutOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);
    const vec2 currentWindowSize = ImGui::GetContentRegionAvail();
    const vec2 utilityPlace = currentWindowSize - 50.0f;
    const float listProportion = 0.30f;
    const vec2 listSize = utilityPlace * vec2(listProportion, 1.0f);
    ImGui::SetCursorPosX(((currentWindowSize - utilityPlace) * 0.5f).x);
    ImGui::BeginChild(ImGui::GetID("Entity templates list"), listSize, true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    if (ImGui::BeginCombo("Filter by world", (worldIdFilter == EEntityWorlds::LENGTH) ? "NO" : EEntityWorldsHelpers::GetName(worldIdFilter)))
    {
        for (u32 i = (u32)EEntityWorlds::STANDARD; i < (u32)EEntityWorlds::LENGTH; ++i)
        {
            if (ImGui::Selectable(EEntityWorldsHelpers::GetName((EEntityWorlds)i), i == (u32)worldIdFilter))
                worldIdFilter = (EEntityWorlds)i;
        }

        if (ImGui::Selectable("NO", EEntityWorlds::LENGTH == worldIdFilter))
            worldIdFilter = EEntityWorlds::LENGTH;

        ImGui::EndCombo();
    }

    static u32 selected = -1;
    forrange(i, 0, templatesToDraw)
    {
        const EntityTemplate* et = EntityTemplateManager::Instance().GetEntityTemplate((u32)i);
        if (worldIdFilter != EEntityWorlds::LENGTH && et->GetWorldId() != worldIdFilter)
            continue;

        if (ImGui::Selectable(et->GetName().c_str(), selected == i))
            selected = (u32)i;
    }
    ImGui::EndChild();

    ImGui::SameLine();

    EntityTemplate* et = (selected != -1) ? EntityTemplateManager::Instance().GetEntityTemplateForWriting(selected) : nullptr;
    const vec2 editorPlace = utilityPlace * vec2(1 - listProportion, 1.0f);
    ImGui::BeginChild(ImGui::GetID("Entity template editor"), editorPlace, true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    if (et != nullptr)
    {
        if (ImGui::Button("Delete template"))
        {
            ImGui::OpenPopup("Delete template modal");
        }

        et->DrawEditor();
        ImGui::Separator();

        const std::map<u32, std::string> modulesList = EntityTemplateManagerMethods::GetModuleList();
        static u32 selectedModule = -1;
        if (ImGui::BeginCombo("##Modulelist", (selectedModule == -1) ? "" : modulesList.at(selectedModule).c_str()))
        {
            foreachitemconst(modName, modulesList)
            {
                bool is_selected = (selectedModule == modName.first);
                if (ImGui::Selectable(modName.second.c_str(), is_selected))
                    selectedModule = modName.first;
                if (is_selected)
                    ImGui::SetItemDefaultFocus(); // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        if (ImGui::Button("Add module") && selectedModule != -1)
        {
            if (!et->HasModule(selectedModule))
            {
                et->SetHasModule(selectedModule);
            }
        }
    }

    bool shouldDeleteTemplate = false;
    if (ImGui::BeginPopupModal("Delete template modal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (ImGui::Button("Yes"))
        {
            shouldDeleteTemplate = true;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("No"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (shouldDeleteTemplate)
    {
        AlwaysCheckedAssert(et != nullptr);
        AlwaysCheckedAssert(selected != -1);

        EntityTemplateManager::Instance().DeleteEntityTemplate_IKNOWWHATIMDOING(selected);
        selected = -1;
    }

    ImGui::EndChild();
    DrawEditorMenu(worldIdFilter, selected);
    ImGui::End();
}

} // namespace ImGUITools
} // namespace ECSEngine
