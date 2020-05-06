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
    const Texture* GetTexture(const TextureHandle& parTextureHandle);

private:
    std::vector<std::unique_ptr<TextureBank>> FTextureBanks;
    std::map<std::string, u32> FTextureBankNameToTextureBankId;
    std::map<std::string, std::string> FTextureBankFileToTextureBankName;
};
} // namespace Rendering
} // namespace ECSEngine
