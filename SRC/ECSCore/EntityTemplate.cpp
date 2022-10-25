#include "stdafx.h"

#include "EntityTemplate.h"

#include "EntityTemplateManager.h"
#include "EntityTemplateManagerMethods.h"
#include "ModuleId.h"
#include "ModuleTemplate.h"
#include "WorldIds.h"

namespace ECSEngine
{

EntityTemplate::EntityTemplate()
    : FWorld(EEntityWorlds::STANDARD)
    , FName("Default")
#ifdef PERFORM_SECURITY_CHECKS
    , FHasBeenInit(false)
#endif
{
}

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

    foreachitem(modIt, FModuleTemplates) { modIt.second->PostLoad(); }

#ifdef PERFORM_SECURITY_CHECKS
    FHasBeenInit = true;
#endif
}

void EntityTemplate::CopyModuleTemplatesMapTo(EntityTemplate* parOther) const
{
    parOther->SetWorldId_IKnowWhatImDoing(FWorld);
    foreachitemconst(modTemp, FModuleTemplates)
    {
        parOther->AddModule(modTemp.first);
    }
}

void EntityTemplate::AddModule(const u32 parId)
{
    auto itFind = FModuleTemplates.find(parId);
    if (itFind == FModuleTemplates.end())
    {
        auto it = FModuleTemplates.emplace(parId, EntityTemplateManagerMethods::CreateModuleTemplate(parId));
        AssertRelease(it.first->second != nullptr);
        it.first->second->Init(this);

#ifdef PERFORM_SECURITY_CHECKS
        it.first->second->VerifyTemplate();
#endif
    }
    else
    {
        itFind->second->Init(this);
#ifdef PERFORM_SECURITY_CHECKS
        itFind->second->VerifyTemplate();
#endif
    }
}

const ModuleTemplate* EntityTemplate::GetModuleTemplate(const u32 parId) const
{
    auto it = FModuleTemplates.find(parId);
    if (it == FModuleTemplates.end())
        return nullptr;
    return it->second.get();
}

void EntityTemplate::DrawEditor()
{
    ImGui::TextColored(vec4(0.8f, 0.8f, 0.8f, 1.0f), "Entity template name:");
    ImGui::SameLine(0.0f, 5.0f);

    char text[256];
    sprintf(text, "%s", FName.c_str());
    ImGui::InputText("", text, 256);
    FName = std::string(text);

    if (ImGui::BeginCombo("Entity world", EEntityWorldsHelpers::GetName(FWorld)))
    {
        for (u32 i = (u32)EEntityWorlds::STANDARD; i < (u32)EEntityWorlds::LENGTH; ++i)
        {
            if (ImGui::Selectable(EEntityWorldsHelpers::GetName((EEntityWorlds)i), i == (u32)FWorld))
                FWorld = (EEntityWorlds)i;
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

void EntityTemplate::UpdateKey()
{
    bool shouldUpdateMap = false;
    foreachitem(modTemplate, FModuleTemplates)
    {
        if (modTemplate.first != modTemplate.second->GetModuleId())
        {
            shouldUpdateMap = true;
        }

        if (!FKey.HasModule(modTemplate.first))
        {
            shouldUpdateMap = true;
        }
    }

    if (shouldUpdateMap)
    {
        FKey.Clear();
        std::map<u32, std::unique_ptr<ModuleTemplate>> newMap;
        foreachitem(modTemplate, FModuleTemplates)
        {
            u32 key = modTemplate.second->GetModuleId();
            FKey.SetHasModule(key);
            newMap.insert_or_assign(key, std::move(modTemplate.second));
        }

        FModuleTemplates = std::move(newMap);
    }
}

template<typename Module>
const ModuleTemplate* EntityTemplate::GetModuleTemplate() const
{
    return GetModuleTemplate(ModuleTraits<Module>::GetModuleId());
}

#define DECLARE_MODULE_AND_TEMPLATE(NAME, TEMPLATE) template const ModuleTemplate* EntityTemplate::GetModuleTemplate<NAME>() const;
#include "ModuleList.inl"
#undef DECLARE_MODULE_AND_TEMPLATE

template<typename Module>
void EntityTemplate::SetHasModule()
{
    FKey.SetHasModule<Module>();
    AddModule(ModuleTraits<Module>::GetModuleId());
}

#define DECLARE_MODULE_AND_TEMPLATE(NAME, TEMPLATE) template void EntityTemplate::SetHasModule<NAME>();
#include "ModuleList.inl"
#undef DECLARE_MODULE_AND_TEMPLATE

} // namespace ECSEngine
