#include "stdafx.h"

#include "MaterialInput.h"

#include "Application/PropertyDrawer.h"
#include "MaterialManager.h"
#include "TexturesManager.h"

namespace ECSEngine
{
namespace Rendering
{

MaterialTextureInputDescriptor::MaterialTextureInputDescriptor()
    : FTextureSamplerName("")
{
}

MaterialTextureInputDescriptor::~MaterialTextureInputDescriptor()
{
}

void MaterialTextureInputDescriptor::DrawEditor()
{
    const auto textureBanks = TextureManager::Instance().GetTextureBanks();

    // combo pour la banque
    if (ImGui::BeginCombo("Texture bank", FTexture.BankName().c_str()))
    {
        ImGui::EndCombo();
    }

    // combo pour la texture dans la banque
    if (ImGui::BeginCombo("Texture name", FTexture.Texture().c_str()))
    {
        ImGui::EndCombo();
    }

    EDITOR_PROPERTY_STRING("Uniform sampler name", FTextureSamplerName, false, "");
    EDITOR_PROPERTY_SIMPLE("Texture slot", FTextureSlot);
}

MaterialTextureInput::MaterialTextureInput(const TextureHandle& parHandle, const std::string& parSlotName, const u32 parSlot)
    : FHandle(parHandle)
    , FTextureSamplerName(parSlotName)
    , FTextureSlot(parSlot)
{
    AssertRelease(FHandle.IsValid());
}

MaterialTextureInput::~MaterialTextureInput()
{
}

void MaterialTextureInput::SetTexture() const
{
    AssertRelease(FHandle.IsValid());
    MaterialManager::SetSamplerUniform(FTextureSamplerName, FHandle, FTextureSlot);
}

} // namespace Rendering
} // namespace ECSEngine
