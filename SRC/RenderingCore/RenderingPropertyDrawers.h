#pragma once
#include "Application/PropertyDrawer.h"
#include "Common/MeshStreamingData.h"
#include "TexturesManager.h"

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

template<>
class PropertyDrawer<Rendering::TextureName>
{
public:
    PropertyDrawer(const std::string& parPropertyName, Rendering::TextureName* parProperty)
        : FPropertyName(parPropertyName)
        , FProperty(parProperty)
    {
    }

    void ShowProperty()
    {
        const auto textureBanks = Rendering::TextureManager::Instance().GetTextureBanks();

        if (!ImGui::CollapsingHeader(FPropertyName.c_str()))
            return;

        // combo pour la banque
        u32 chosenBank = -1;
        forrange(i, 0, textureBanks.size())
        {
            if (textureBanks[i]->TextureBankName() == FProperty->BankName())
            {
                chosenBank = i;
                break;
            }
        }

        bool bankHasChanged = chosenBank == -1;
        chosenBank = (chosenBank == -1) ? 0 : chosenBank;
        if (ImGui::BeginCombo("Choose bank", FProperty->BankName().c_str()))
        {
            forrange(i, 0, textureBanks.size())
            {
                const bool isCurrent = i == chosenBank;
                if (ImGui::Selectable(textureBanks[i]->TextureBankName().c_str(), isCurrent))
                {
                    chosenBank = i;
                    bankHasChanged = !isCurrent;
                }
            }
            ImGui::EndCombo();
        }

        // combo pour la texture dans la banque
        u32 chosenTexture = (bankHasChanged) ? 0 : -1;
        std::string textureName = "";
        if (!bankHasChanged)
        {
            auto textureBank = textureBanks[chosenBank].get();
            u32 i = 0;
            foreachitemconst(texture, textureBank->Descriptors())
            {
                if (FProperty->Texture() == texture.first)
                {
                    chosenTexture = i;
                    textureName = texture.first;
                    break;
                }
                ++i;
            }
        }

        bool textureHasChanged = bankHasChanged;
        if (ImGui::BeginCombo("Choose texture", FProperty->Texture().c_str()))
        {
            auto textureBank = textureBanks[chosenBank].get();
            u32 i = 0;
            foreachitemconst(texture, textureBank->Descriptors())
            {
                const bool isCurrent = i == chosenTexture;
                if (ImGui::Selectable(texture.first.c_str(), isCurrent))
                {
                    chosenTexture = i;
                    textureHasChanged = !isCurrent;
                    textureName = texture.first;
                }
                ++i;
            }
            ImGui::EndCombo();
        }

        if (chosenBank || chosenTexture)
        {
            *FProperty = Rendering::TextureName(textureBanks[chosenBank]->TextureBankName(), textureName);
        }
    }

private:
    std::string FPropertyName;
    Rendering::TextureName* FProperty;
};

#define EDITOR_PROPERTY_TEXTURE_NAME(NAME, PROPERTY)                                                                                                                               \
    {                                                                                                                                                                              \
        PropertyDrawer<Rendering::TextureName> drawer(NAME, &PROPERTY);                                                                                                            \
        drawer.ShowProperty();                                                                                                                                                     \
    }
} // namespace ECSEngine
