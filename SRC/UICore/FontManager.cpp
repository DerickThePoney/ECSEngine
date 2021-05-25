#include "stdafx.h"

#include "FontManager.h"

#include "Common/RenderingHandles.h"
#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
#include "Common/Singleton.h"
#include "Font.h"

namespace ECSEngine
{
namespace UI
{
class FontManager : public Singleton<FontManager>
{
public:
    void Initialise(const std::string& parFontsConfigurationFile);
    void SaveFontFamiles(const std::string& parFontsConfigurationFile);
    void Shutdown();

    const Rendering::TextureHandle& FontAtlas() const { return FFontAtlas; };
    const FontFamilies& GetFontFamilies() const { return FFontFamilies; }

    const Font* GetFont(const FontFamilyName& parName, const FontSize parFontSize);

private:
    void LoadFontsFamilies();

private:
    FontsDatabase FFontDatabase;
    FontFamilies FFontFamilies;
    Rendering::TextureHandle FFontAtlas;
};

void FontManager::Initialise(const std::string& parFontsConfigurationFile)
{
    Resource fontConf(parFontsConfigurationFile);
    std::shared_ptr<ResourceHandle> fileHandle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&fontConf);
    AlwaysCheckedAssert(fileHandle != nullptr);
    if (fileHandle != nullptr)
    {
        ResourceBuffer buff = fileHandle->GetResourceBuffer();
        std::istream istr(&buff, std::istream::in);
        cereal::JSONInputArchive archive(istr);
        archive(NAMEDPROPERTY("FontFamilies", FFontFamilies));
    }
}

void FontManager::SaveFontFamiles(const std::string& parFontsConfigurationFile)
{
    std::ofstream ofstr(GlobalResourceCache::Instance().FCache->GetBasePath() + '\\' + parFontsConfigurationFile);
    AssertRelease(ofstr.good());
    cereal::JSONOutputArchive archive(ofstr);
    archive(NAMEDPROPERTY("FontFamilies", FFontFamilies));
}

void FontManager::Shutdown()
{
}

const Font* FontManager::GetFont(const FontFamilyName& parName, const FontSize parFontSize)
{
    auto itFind = FFontDatabase.find(parName);
    if (itFind == FFontDatabase.end())
        return nullptr;

    auto itFind2 = itFind->second.find(parFontSize);
    if (itFind2 == itFind->second.end())
        return nullptr;
    return &itFind2->second;
}

void FontManager::LoadFontsFamilies()
{
}

namespace Fonts
{

void CreateFontManager()
{
    AssertRelease(!FontManager::HasInstance());
    FontManager::CreateIFP();
}

void InitialiseFontManager(const std::string& parFontsConfigurationFile)
{
    AssertRelease(FontManager::HasInstance());
    FontManager::Instance().Initialise(parFontsConfigurationFile);
}

void SaveFontFamiles(const std::string& parFontsConfigurationFile)
{
}

void ShutdownFontManager()
{
    FontManager::Destroy();
}

const FontFamilies& GetFontFamilies()
{
    return FontManager::Instance().GetFontFamilies();
}

const Font* GetFont(const FontFamilyName& parName, const FontSize parFontSize)
{
    return nullptr;
}

const Rendering::TextureHandle& FontAtlas()
{
    return FontManager::Instance().FontAtlas();
}

} // namespace Fonts
} // namespace UI
} // namespace ECSEngine
