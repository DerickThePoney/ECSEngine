#pragma once
#include "Common/RenderingHandles.h"
#include "TextureDescriptor.h"

namespace ECSEngine
{
namespace Rendering
{
class Texture;
class TextureBank
{
public:
    TextureBank(const u32 parBankId);
    ~TextureBank();

    void LoadBankIFP();
    void UnloadBankIFP();

    const std::string& TextureBankName() const { return FTextureBankName; }
    const u32 BankId() const { return FBankId; }
    const std::map<std::string, TextureDescriptor>& Descriptors() { return FDescriptors; }

    void SetTextureBankName(const std::string& parName) { FTextureBankName = parName; }
    void SetDescriptors(const std::map<std::string, TextureDescriptor>& parDescriptors) { FDescriptors = parDescriptors; }

    const TextureHandle GetTextureHandle(const TextureName& parTexture);
    const Texture* GetTexture(const TextureHandle& parHandle);

    SERIALIZE() { ar(PROPERTY(TextureBankName), NAMEDPROPERTY("Textures", FDescriptors)); }

private:
    std::string FTextureBankName;
    std::map<std::string, TextureDescriptor> FDescriptors;

    std::vector<std::unique_ptr<Texture>> FTextures;
    std::map<std::string, u32> FTextureNameToTextureIndex;
    u32 FBankId;

    bool FLoaded;
};
} // namespace Rendering
} // namespace ECSEngine