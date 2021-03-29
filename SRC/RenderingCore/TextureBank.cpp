#include "stdafx.h"

#include "TextureBank.h"

#include "Texture.h"

namespace ECSEngine
{
namespace Rendering
{

TextureBank::TextureBank(const u32 parBankId)
    : FBankId(parBankId)
    , FLoaded(false)
{
}

TextureBank::~TextureBank()
{
    UnloadBankIFP();
}

void TextureBank::LoadBankIFP()
{
    if (FLoaded)
        return;

    foreachitemconst(descriptor, FDescriptors)
    {
        const u32 idx = (u32)FTextures.size();
        FTextureNameToTextureIndex[descriptor.first] = idx;
        FTextures.push_back(std::unique_ptr<Texture>(new Texture(&descriptor.second, descriptor.first)));
        FTextures.back()->Load();
        AssertRelease(FTextures.back()->Valid());
    }
    FLoaded = true;
}

void TextureBank::UnloadBankIFP()
{
    if (!FLoaded)
        return;

    FTextureNameToTextureIndex.clear();
    FTextures.clear();
    FLoaded = false;
}

const TextureHandle TextureBank::GetTextureHandle(const TextureName& parTexture)
{
    AssertRelease(parTexture.BankName() == FTextureBankName);

#ifdef PERFORM_SECURITY_CHECKS
    auto it = FDescriptors.find(parTexture.Texture());
    AssertRelease(it != FDescriptors.end());
#endif

    if (!FLoaded)
        LoadBankIFP();

    auto itIdx = FTextureNameToTextureIndex.find(parTexture.Texture());
    AssertRelease(itIdx != FTextureNameToTextureIndex.end());

    AssertRelease(FTextures.size() > itIdx->second);
    AssertRelease(FTextures[itIdx->second] != nullptr);
    return TextureHandle(FBankId, itIdx->second);
}

const Texture* TextureBank::GetTexture(const TextureHandle& parHandle)
{
    AssertRelease(parHandle.IsValid());
    AssertRelease(parHandle.GetBankId() == FBankId);
    AssertRelease(parHandle.GetTextureId() < FTextures.size());
    AssertRelease(FTextures[parHandle.GetTextureId()] != nullptr);
    return FTextures[parHandle.GetTextureId()].get();
}

} // namespace Rendering
} // namespace ECSEngine
