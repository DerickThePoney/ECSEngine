#include "stdafx.h"

#include "TexturesManager.h"

#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceFile.h"
#include "Common/ResourceHandle.h"
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

const Texture* TextureManager::GetTexture(const TextureHandle& parTextureHandle)
{
    AssertRelease(FTextureBanks[parTextureHandle.GetBankId()] != nullptr);
    return FTextureBanks[parTextureHandle.GetBankId()]->GetTexture(parTextureHandle);
}

} // namespace Rendering
} // namespace ECSEngine