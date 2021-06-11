#pragma once

namespace ECSEngine
{
namespace Rendering
{
class TextureHandle;
}
namespace UI
{
class Font;
using FontFamilyName = std::string;
using FontSize = float;
using FontsDatabase = std::unordered_map<FontFamilyName, Font>;
using FontFamiliesSizes = std::unordered_map<FontFamilyName, std::set<FontSize>>;
namespace Fonts
{
void CreateFontManager();
void InitialiseFontManager(const std::string& parFontsConfigurationFile);
void SaveFontFamiles();
void ShutdownFontManager();

FontFamiliesSizes& GetFontFamilies();
const Font* GetFont(const FontFamilyName& parName, const FontSize parFontSize);
} // namespace Fonts
} // namespace UI
} // namespace ECSEngine
