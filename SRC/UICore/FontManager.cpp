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

    const FontFamiliesSizes& GetFontFamilies() const { return FFontFamilies; }

    const Font* GetFont(const FontFamilyName& parName, const FontSize parFontSize);

private:
    void LoadFontsFamilies();

private:
    FontsDatabase FFontDatabase;
    FontFamiliesSizes FFontFamilies;
    const Font* FDefaultFont = nullptr;
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
    else
    {
        AlwaysCheckedAssert(FFontFamilies.empty());
        std::set<float> sizes;
        sizes.insert(30.f);
        FFontFamilies.insert_or_assign("Fonts\\OpenSans-Regular.ttf", sizes);
        SaveFontFamiles(parFontsConfigurationFile);
    }

    LoadFontsFamilies();

    FDefaultFont = GetFont("Fonts\\OpenSans-Regular.ttf", 30.f);
    AssertRelease(FDefaultFont != nullptr);
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
    AlwaysCheckedAssert(itFind != FFontDatabase.end());
    if (itFind == FFontDatabase.end())
        return FDefaultFont;

#ifdef ENABLE_SECURITY_CHECKS
    auto itFindSize = FFontFamilies.find(parName);
    AlwaysCheckedAssert(itFindSize != FFontFamilies.end());
    AlwaysCheckedAssert(itFindSize->second.find(parFontSize) != itFindSize->second.end());
#endif

    return &itFind->second;
}

void FontManager::LoadFontsFamilies()
{
    // Algo -- Scrap that, each font is an atlas, we pass it the list of size, and go on with...
    foreachitemconst(fontFamily, FFontFamilies)
    {
        auto it = FFontDatabase.insert_or_assign(fontFamily.first, Font());
        Resource r(fontFamily.first);
        it.first->second.InitFromResource(r, fontFamily.second);
    }
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
    AssertRelease(FontManager::HasInstance());
    FontManager::Instance().SaveFontFamiles(parFontsConfigurationFile);
}

void ShutdownFontManager()
{
    AssertRelease(FontManager::HasInstance());
    FontManager::Destroy();
}

const FontFamiliesSizes& GetFontFamilies()
{
    AssertRelease(FontManager::HasInstance());
    return FontManager::Instance().GetFontFamilies();
}

const Font* GetFont(const FontFamilyName& parName, const FontSize parFontSize)
{
    AssertRelease(FontManager::HasInstance());
    return FontManager::Instance().GetFont(parName, parFontSize);
}

} // namespace Fonts
} // namespace UI
} // namespace ECSEngine
