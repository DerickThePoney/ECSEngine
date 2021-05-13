#include "stdafx.h"

#include "FontTextureManager.h"

#include "Common/RenderingHandles.h"
#include "Texture.h"

namespace ECSEngine
{
namespace Rendering
{

FontTextureManager::~FontTextureManager()
{
    foreachitem(texture, FFontTextures) delete texture;
}

TextureHandle FontTextureManager::AddNewFontTexture(const std::string& parName, const u32 parWidth, const u32 parHeight, const uc8* parTextureData)
{
    bgfx::TextureHandle handle = bgfx::createTexture2D((u16)parWidth, (u16)parHeight, false, 1, bgfx::TextureFormat::A8, 0, bgfx::copy(parTextureData, parWidth * parHeight));

    AssertRelease(bgfx::isValid(handle));
    bgfx::TextureInfo info;
    info.bitsPerPixel = 8;
    info.format = bgfx::TextureFormat::A8;
    info.numLayers = 1;
    info.height = parHeight;
    info.width = parWidth;
    info.numMips = 1;

    FFontTextures.push_back(new Texture(nullptr, parName));
    FFontTextures.back()->SetTextureData_IKnowWhatImDoing(handle, info);

    return TextureHandle(-1, (u32)FFontTextures.size() - 1);
}

const Texture* FontTextureManager::GetFontTexture(const TextureHandle& parHandle) const
{
    AssertRelease(parHandle.GetBankId() == -1);
    AssertRelease(parHandle.GetTextureId() < FFontTextures.size());

    return FFontTextures[parHandle.GetTextureId()];
}

} // namespace Rendering
} // namespace ECSEngine