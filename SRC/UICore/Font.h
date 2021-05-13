#pragma once
#include "Common/RenderingHandles.h"

namespace ECSEngine
{
class Resource;
namespace UI
{
struct FontGlyph
{
    u32 codePoint = 0;
    float x0 = 0;
    float y0 = 0;
    float x1 = 0;
    float y1 = 0;
    float u0 = 0;
    float v0 = 0;
    float u1 = 0;
    float v1 = 0;

    float advance = 0.f;
};

class Font
{
public:
    void InitFromResource(const Resource& parRes);

    float FontSize() const { return FFontSize; }

    void PushGlyph(FontGlyph parGlyph)
    {
        FCodepointToGlyphMap[parGlyph.codePoint] = FGlyphs.size();
        FGlyphs.push_back(parGlyph);
    }

    const FontGlyph* GetGlyph(u32 codePoint);

private:
    float FFontSize = 30.f;

    std::vector<FontGlyph> FGlyphs;
    std::unordered_map<u32, u32> FCodepointToGlyphMap;
    Rendering::TextureHandle FFontTexture;
};
} // namespace UI
} // namespace ECSEngine