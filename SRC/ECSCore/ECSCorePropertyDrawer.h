#pragma once

#include "Application/PropertyDrawer.h"
#include "ECSCore/WorldIds.h"
#include "EntityTemplate.h"
#include "EntityTemplateManager.h"

namespace ECSEngine
{
template<>
class PropertyDrawer<const EntityTemplate*>
{
public:
    PropertyDrawer(const std::string& parPropertyName, const EntityTemplate** parProperty, std::string* parNameProperty, EEntityWorlds parFilter = EEntityWorlds::LENGTH)
        : FName(parPropertyName)
        , FProperty(parProperty)
        , FNameProperty(parNameProperty)
        , FFilter(parFilter)
    {
    }

    void ShowProperty()
    {
        const u32 templatesNumber = EntityTemplateManager::Instance().GetEntityTemplatesNumber();
        u32 selected = -1;

        if (FProperty != nullptr)
        {
            forrange(i, 0, templatesNumber)
            {
                if (*FProperty == EntityTemplateManager::Instance().GetEntityTemplate((u32)i))
                {
                    selected = (u32)i;
                    break;
                }
            }
        }

        if (ImGui::BeginCombo(FName.c_str(), (selected != -1) ? (*FProperty)->GetName().c_str() : "No associated template"))
        {

            forrange(i, 0, templatesNumber)
            {
                const EntityTemplate* temp = EntityTemplateManager::Instance().GetEntityTemplate((u32)i);

                if (FFilter != EEntityWorlds::LENGTH && temp->GetWorldId() != FFilter)
                    continue;

                bool is_selected = (selected == (u32)i);
                if (ImGui::Selectable(temp->GetName().c_str(), is_selected))
                    selected = (u32)i;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        if (selected != -1)
        {
            *FProperty = EntityTemplateManager::Instance().GetEntityTemplate((u32)selected);
            AssertRelease(*FProperty != nullptr);
            *FNameProperty = (*FProperty)->GetName();
        }
    }

private:
    std::string FName;
    const EntityTemplate** FProperty = nullptr;
    std::string* FNameProperty = nullptr;
    EEntityWorlds FFilter = EEntityWorlds::LENGTH;
};

#define EDITOR_PROPERTY_ENTITY_TEMPLATE(NAME, PROPERTY, NAME_PROPERTY)                                                                                                             \
    {                                                                                                                                                                              \
        PropertyDrawer<const EntityTemplate*> drawer(NAME, &PROPERTY, &NAME_PROPERTY);                                                                                             \
        drawer.ShowProperty();                                                                                                                                                     \
    }

#define EDITOR_PROPERTY_ENTITY_TEMPLATE_FILTERED(NAME, PROPERTY, NAME_PROPERTY, FILTER)                                                                                            \
    {                                                                                                                                                                              \
        PropertyDrawer<const EntityTemplate*> drawer(NAME, &PROPERTY, &NAME_PROPERTY, FILTER);                                                                                     \
        drawer.ShowProperty();                                                                                                                                                     \
    }

} // namespace ECSEngine
