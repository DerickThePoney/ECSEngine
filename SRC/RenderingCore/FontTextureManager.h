#pragma once
#include "Common/Singleton.h"
namespace ECSEngine
{
namespace Rendering
{
class TextureHandle;
class Texture;
class FontTextureManager : public Singleton<FontTextureManager>
{
public:
    ~FontTextureManager();
    TextureHandle AddNewFontTexture(const std::string& parName, const u32 parWidth, const u32 parHeight, const uc8* parTextureData);

    const Texture* GetFontTexture(const TextureHandle& parHandle) const;

private:
    std::vector<Texture*> FFontTextures;
};
} // namespace Rendering
} // namespace ECSEngine