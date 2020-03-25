#pragma once

#include "Application/PropertyDrawer.h"
#include "EntityTemplate.h"

namespace ECSEngine
{
template<>
class PropertyDrawer<const EntityTemplate*>
{
public:
    PropertyDrawer(const std::string& parPropertyName, const EntityTemplate* parProperty, std::string* parNameProperty)
        : FName(parPropertyName)
        , FProperty(parProperty)
    {
    }

    void ShowProperty()
    { /*ImGui::InputFloat4(FName.c_str(), (float*)FProperty); --> TODO*/
    }

private:
    std::string FName;
    const EntityTemplate* FProperty = nullptr;
    std::string* FNameProperty = nullptr;
};

#define EDITOR_PROPERTY_ENTITY_TEMPLATE(NAME, PROPERTY, NAME_PROPERTY)                                                                                                             \
    {                                                                                                                                                                              \
        PropertyDrawer<const EntityTemplate*> drawer(NAME, PROPERTY, &NAME_PROPERTY);                                                                                              \
        drawer.ShowProperty();                                                                                                                                                     \
    }
} // namespace ECSEngine