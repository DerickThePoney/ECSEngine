#include "stdafx.h"

#include "EntityTemplate.h"

#include "EntityTemplateManager.h"

namespace ECSEngine
{

EntityTemplate::~EntityTemplate()
{
}

void EntityTemplate::Initialise()
{
    for (u32 i = 0; i < (u32)EModuleId::Length; ++i)
    {
        if (HasModule(i))
        {
            AddModule(i);
        }
    }

#ifdef PERFORM_SECURITY_CHECKS
    FHasBeenInit = true;
#endif
}

void EntityTemplate::AddModule(const u32 parId)
{
    auto itFind = FModuleTemplates.find(parId);
    if (itFind == FModuleTemplates.end())
    {
        auto& it = FModuleTemplates.emplace(parId, EntityTemplateManagerMethods::CreateModuleTemplate(parId));
        AssertRelease(it.first->second != nullptr);
        it.first->second->Init(this);
    }
    else
    {
        itFind->second->Init(this);
    }
}

void EntityTemplate::DrawEditor()
{
    ImGui::TextColored(glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), "Entity template name:");
    ImGui::SameLine(0.0f, 5.0f);

    char text[256];
    sprintf(text, "%s", FName.c_str());
    ImGui::InputText("", text, 256);
    FName = std::string(text);

    if (ImGui::BeginCombo("Entity world", Worlds::GetName(FWorld)))
    {
        for (u32 i = Worlds::STANDARD; i < Worlds::LENGTH; ++i)
        {
            if (ImGui::Selectable(Worlds::GetName((Worlds::Type)i), i == FWorld))
                FWorld = (Worlds::Type)i;
        }
        ImGui::EndCombo();
    }

    std::vector<u32> modulesToRemove;
    foreachitem(itModulesTemplates, FModuleTemplates)
    {
        if (itModulesTemplates.second->DrawEditor())
        {
            modulesToRemove.push_back(itModulesTemplates.first);
        }
    }

    foreachitemconst(id, modulesToRemove)
    {
        FModuleTemplates.erase(id);
        RemoveModule(id);
    }
}

} // namespace ECSEngine
