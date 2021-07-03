#pragma once
#include "Common/RenderingHandles.h"
#include "Common/Singleton.h"
#include "TextureBank.h"

namespace ECSEngine
{
namespace Rendering
{
class Texture;
class TextureManager : public Singleton<TextureManager>
{
public:
    TextureManager();
    ~TextureManager();

    void Initialise();
    void Shutdown();

    const TextureHandle GetTextureHandle(const TextureName& parTextureName);
    const Texture* GetTexture(const TextureHandle& parTextureHandle) const;

    const u32 CreateFreeFormTexture(const std::string parFilename);
    const u32 CreateFreeFormTexture(const u8* parData, u32 parWidth, u32 parHeight);
    void ReleaseFreeFormTexture(const u32 parId);
    const Texture* GetFreeFormTexture(const u32 parId) const;

private:
    std::vector<std::unique_ptr<TextureBank>> FTextureBanks;
    std::map<std::string, u32> FTextureBankNameToTextureBankId;
    std::map<std::string, std::string> FTextureBankFileToTextureBankName;

    std::vector<std::unique_ptr<Texture>> FFreeFormTextures;
};
} // namespace Rendering
} // namespace ECSEngine
