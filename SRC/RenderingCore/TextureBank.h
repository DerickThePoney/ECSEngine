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
    TextureBank();
    ~TextureBank();

    void LoadBankIFP();
    void UnloadBankIFP();

    const std::string& TextureBankName() const { return FTextureBankName; }
    const std::map<std::string, TextureDescriptor>& Descriptors() { return FDescriptors; }

    void SetTextureBankName(const std::string& parName) { FTextureBankName = parName; }
    void SetDescriptors(const std::map<std::string, TextureDescriptor>& parDescriptors) { FDescriptors = parDescriptors; }

    const TextureHandle GetTextureHandle(const TextureName& parTexture);
    const Texture* GetTexture(const TextureHandle& parHandle);

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(PROPERTY(TextureBankName), NAMEDPROPERTY("Textures", FDescriptors));
    }

private:
    std::string FTextureBankName;
    std::map<std::string, TextureDescriptor> FDescriptors;

    std::vector<std::unique_ptr<Texture>> FTextures;
    std::map<std::string, u32> FTextureNameToTextureIndex;

    bool FLoaded;
};
} // namespace Rendering
} // namespace ECSEngine