#include "stdafx.h"

#include "TexturesManager.h"

#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceFile.h"
#include "Common/ResourceHandle.h"
#include "FontTextureManager.h"
#include "Texture.h"
namespace ECSEngine
{
namespace Rendering
{

TextureManager::TextureManager()
    : Singleton()
{
}

TextureManager::~TextureManager()
{
    FTextureBanks.clear();
    FTextureBankNameToTextureBankId.clear();
    FTextureBankFileToTextureBankName.clear();
}

void TextureManager::Initialise()
{
    std::vector<std::string> textureBanksFiles;
    GlobalResourceCache::Instance().FCache->GetFileSystem()->ListResourceFiles("*.texturebank", textureBanksFiles);

    foreachitemconst(file, textureBanksFiles)
    {
        AssertRelease(FTextureBankFileToTextureBankName.find(file) == FTextureBankFileToTextureBankName.end());

        Resource r(file);
        std::shared_ptr<ResourceHandle> handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&r);
        AssertRelease(handle != nullptr);

        TextureBank* bank = new TextureBank((u32)FTextureBanks.size());
        {
            ResourceBuffer buf = handle->GetResourceBuffer();
            std::istream isstr(&buf, std::istream::in);

            cereal::JSONInputArchive ar(isstr);
            ar(NAMEDPROPERTY("TextureBank", *bank));
        }

        AssertRelease(FTextureBankNameToTextureBankId.find(bank->TextureBankName()) == FTextureBankNameToTextureBankId.end());
        AssertRelease(FTextureBankFileToTextureBankName.find(file) == FTextureBankFileToTextureBankName.end());
        FTextureBankNameToTextureBankId[bank->TextureBankName()] = bank->BankId();
        FTextureBankFileToTextureBankName[file] = bank->TextureBankName();
        FTextureBanks.push_back(std::unique_ptr<TextureBank>(bank));
    }
}

void TextureManager::Shutdown()
{
    FTextureBanks.clear();
    FTextureBankNameToTextureBankId.clear();
    FTextureBankFileToTextureBankName.clear();
}

const TextureHandle TextureManager::GetTextureHandle(const TextureName& parTextureName)
{
    auto it = FTextureBankNameToTextureBankId.find(parTextureName.BankName());
    AssertRelease(it != FTextureBankNameToTextureBankId.end());
    AssertRelease(FTextureBanks[it->second] != nullptr);
    return FTextureBanks[it->second]->GetTextureHandle(parTextureName);
}

const Texture* TextureManager::GetTexture(const TextureHandle& parTextureHandle) const
{
    if (parTextureHandle.GetBankId() == -1)
        return FontTextureManager::Instance().GetFontTexture(parTextureHandle);
    AssertRelease(FTextureBanks[parTextureHandle.GetBankId()] != nullptr);
    return FTextureBanks[parTextureHandle.GetBankId()]->GetTexture(parTextureHandle);
}

const u32 TextureManager::CreateFreeFormTexture(const std::string parFilename)
{
    u32 index = -1;
    forrange(i, 0, FFreeFormTextures.size())
    {
        if (FFreeFormTextures[i] == nullptr)
        {
            index = i;
            break;
        }
    }
    if (index == -1)
    {
        index = FFreeFormTextures.size();
        FFreeFormTextures.resize(FFreeFormTextures.size() + 1);
    }

    FFreeFormTextures[index].reset(new Texture(nullptr, parFilename));
    FFreeFormTextures[index]->LoadFromTextureFile(BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MIN_POINT);
    return index;
}

const u32 TextureManager::CreateFreeFormTexture(const u8* parData, u32 parWidth, u32 parHeight)
{
    bgfx::TextureHandle handle = bgfx::createTexture2D((u16)parWidth, (u16)parHeight, false, 1, bgfx::TextureFormat::RGBA8, 0, bgfx::copy(parData, parWidth * parHeight * 4));

    AssertRelease(bgfx::isValid(handle));
    bgfx::TextureInfo info;
    info.bitsPerPixel = 32;
    info.format = bgfx::TextureFormat::RGBA8;
    info.numLayers = 1;
    info.height = parHeight;
    info.width = parWidth;
    info.numMips = 1;

    u32 index = -1;
    forrange(i, 0, FFreeFormTextures.size())
    {
        if (FFreeFormTextures[i] == nullptr)
        {
            index = i;
            break;
        }
    }
    if (index == -1)
    {
        index = FFreeFormTextures.size();
        FFreeFormTextures.resize(FFreeFormTextures.size() + 1);
    }

    FFreeFormTextures[index].reset(new Texture(nullptr, "FreeFormFromData"));
    FFreeFormTextures[index]->SetTextureData_IKnowWhatImDoing(handle, info);
    return index;
}

void TextureManager::ReleaseFreeFormTexture(const u32 parId)
{
    if (parId >= FFreeFormTextures.size())
        return;
    FFreeFormTextures[parId].reset(nullptr);
}

const Texture* TextureManager::GetFreeFormTexture(const u32 parId) const
{
    if (parId >= FFreeFormTextures.size())
        return nullptr;
    return FFreeFormTextures[parId].get();
}

} // namespace Rendering
} // namespace ECSEngine
