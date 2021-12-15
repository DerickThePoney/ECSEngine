#pragma once
#include "Application/PropertyDrawer.h"
#include "Common/MeshStreamingData.h"

namespace ECSEngine
{
template<>
class PropertyDrawer<Rendering::MeshLayoutDescription>
{
public:
    PropertyDrawer(const std::string& parPropertyName, Rendering::MeshLayoutDescription* parProperty)
        : FPropertyName(parPropertyName)
        , FProperty(parProperty)
    {
    }

    void ShowProperty()
    {
        ImGui::Text(FPropertyName.c_str());

        EDITOR_PROPERTY_BOOL("HasPositions", FProperty->HasPositions);
        EDITOR_PROPERTY_BOOL("HasColors", FProperty->HasColors);
        EDITOR_PROPERTY_SIMPLE("NbColorChannels", FProperty->NbColorChannels);
        EDITOR_PROPERTY_BOOL("HasUVs", FProperty->HasUVs);
        EDITOR_PROPERTY_SIMPLE("NbUVs", FProperty->NbUVs);
        EDITOR_PROPERTY_BOOL("HasNormals", FProperty->HasNormals);
        EDITOR_PROPERTY_BOOL("HasTangents", FProperty->HasTangents);
        EDITOR_PROPERTY_BOOL("HasBinormals", FProperty->HasBinormals);
        EDITOR_PROPERTY_BOOL("HasBones", FProperty->HasBones);
    }

private:
    std::string FPropertyName;
    Rendering::MeshLayoutDescription* FProperty;
};

#define EDITOR_PROPERTY_MESH_LAYOUT_DESCRIPTION(NAME, PROPERTY)                                                                                                                    \
    {                                                                                                                                                                              \
        PropertyDrawer<Rendering::MeshLayoutDescription> drawer(NAME, &PROPERTY);                                                                                                  \
        drawer.ShowProperty();                                                                                                                                                     \
    }
} // namespace ECSEngine
