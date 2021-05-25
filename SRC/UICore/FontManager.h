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
using FontsDatabase = std::unordered_map<FontFamilyName, std::unordered_map<FontSize, Font>>;
using FontFamilies = std::unordered_map<FontFamilyName, std::set<FontSize>>;
namespace Fonts
{
void CreateFontManager();
void InitialiseFontManager(const std::string& parFontsConfigurationFile);
void SaveFontFamiles(const std::string& parFontsConfigurationFile);
void ShutdownFontManager();

const FontFamilies& GetFontFamilies();
const Font* GetFont(const FontFamilyName& parName, const FontSize parFontSize);
const Rendering::TextureHandle& FontAtlas();
} // namespace Fonts
} // namespace UI
} // namespace ECSEngine
