#pragma once
#include "Common/RenderingHandles.h"
namespace ECSEngine
{
namespace Rendering
{
class MaterialTextureInputDescriptor
{
public:
    MaterialTextureInputDescriptor();
    ~MaterialTextureInputDescriptor();

    const TextureName& GetTexture() const { return FTexture; }
    const std::string& GetTextureSlotName() const { return FTextureSamplerName; }
    const u32 GetSlot() const { return FTextureSlot; }

    SERIALIZE() { ar(PROPERTY(Texture), PROPERTY(TextureSamplerName), PROPERTY(TextureSlot)); }

    void DrawEditor();

private:
    TextureName FTexture;
    std::string FTextureSamplerName;
    u32 FTextureSlot;
};

class MaterialTextureInput
{
public:
    MaterialTextureInput(const TextureHandle& parHandle, const std::string& parSlotName, const u32 parSlot);
    ~MaterialTextureInput();

    virtual void SetTexture() const;

private:
    TextureHandle FHandle;
    std::string FTextureSamplerName;
    u32 FTextureSlot;
};
} // namespace Rendering
} // namespace ECSEngine
