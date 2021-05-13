#include "stdafx.h"

#include "Font.h"

#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
#include "RenderingCore/FontTextureManager.h"

#ifndef STB_RECT_PACK_IMPLEMENTATION
#define STB_RECT_PACK_IMPLEMENTATION
#define STBRP_STATIC
#include "imgui/imstb_rectpack.h"
#endif

#ifndef STB_TRUETYPE_IMPLEMENTATION // in case the user already have an implementation in the _same_ compilation unit (e.g. unity builds)
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "imgui/imstb_truetype.h"
#endif

namespace ECSEngine
{
namespace UI
{
static const u16 UTFCodePoints[] = { 0x0020, 0x00FF };
constexpr i32 FontTexturePadding = 1;
constexpr i32 HOversample = 3;
constexpr i32 VOversample = 1;

void Font::InitFromResource(const Resource& parRes)
{
    std::shared_ptr<ResourceHandle> resHandle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&parRes);
    AssertRelease(resHandle != nullptr);

    // font init
    stbtt_fontinfo fontInfo;
    const int nbFont = stbtt_GetNumberOfFonts((unsigned char*)resHandle->Buffer());
    const int fontOffset = stbtt_GetFontOffsetForIndex((unsigned char*)resHandle->Buffer(), 0);
    bool success = stbtt_InitFont(&fontInfo, (unsigned char*)resHandle->Buffer(), fontOffset) != 0;
    AssertRelease(success);

    // get available code points
    std::vector<i32> codepoints;
    forrange(code, UTFCodePoints[0], UTFCodePoints[1])
    {
        if (!stbtt_FindGlyphIndex(&fontInfo, code))
            continue;

        codepoints.push_back(code);
    }

    // rect packing
    std::vector<stbrp_rect> glyphRects;
    std::vector<stbtt_packedchar> packedChars;
    glyphRects.resize(codepoints.size());
    packedChars.resize(codepoints.size());

    stbtt_pack_range packRange;
    packRange.font_size = FFontSize;
    packRange.first_unicode_codepoint_in_range = 0;
    packRange.array_of_unicode_codepoints = codepoints.data();
    packRange.num_chars = codepoints.size();
    packRange.chardata_for_range = &packedChars[0];
    packRange.h_oversample = HOversample;
    packRange.v_oversample = VOversample;

    const float scale = stbtt_ScaleForPixelHeight(&fontInfo, FFontSize);
    float surface = 0.f;
    forrange(i, 0, codepoints.size())
    {
        i32 x0, y0, x1, y1;
        const i32 glyphIndexInFont = stbtt_FindGlyphIndex(&fontInfo, codepoints[i]);
        AssertRelease(glyphIndexInFont != 0);
        stbtt_GetGlyphBitmapBoxSubpixel(&fontInfo, glyphIndexInFont, scale * HOversample, scale * VOversample, 0, 0, &x0, &y0, &x1, &y1);
        glyphRects[i].w = (stbrp_coord)(x1 - x0 + FontTexturePadding + HOversample - 1);
        glyphRects[i].h = (stbrp_coord)(y1 - y0 + FontTexturePadding + VOversample - 1);
        surface += glyphRects[i].w * glyphRects[i].h;
    }

    const i32 surface_sqrt = (i32)glm::sqrt(surface);
    const i32 wantedWidth = (surface_sqrt >= 4096 * 0.7f) ? 4096 : (surface_sqrt >= 2048 * 0.7f) ? 2048 : (surface_sqrt >= 1024 * 0.7f) ? 1024 : 512;
    static constexpr i32 maxHeight = 1024 * 32;

    stbtt_pack_context spc = {};
    stbtt_PackBegin(&spc, NULL, wantedWidth, maxHeight, 0, FontTexturePadding, NULL);

    stbrp_pack_rects((stbrp_context*)spc.pack_info, glyphRects.data(), glyphRects.size());

    int wantedHeight = 0;
    foreachitem(rect, glyphRects)
    {
        if (rect.was_packed)
            wantedHeight = glm::max(wantedHeight, rect.y + rect.h);
    }

    wantedHeight = glm::nextPowerOfTwo(wantedHeight);

    u8* textureData = new u8[wantedHeight * wantedWidth];
    memset(textureData, 0, wantedWidth * wantedHeight);
    spc.pixels = textureData;
    spc.height = wantedHeight;

    stbtt_PackFontRangesRenderIntoRects(&spc, &fontInfo, &packRange, 1, glyphRects.data());

    stbtt_PackEnd(&spc);

    // load glyphs

    int unscaled_ascent, unscaled_descent, unscaled_line_gap;
    stbtt_GetFontVMetrics(&fontInfo, &unscaled_ascent, &unscaled_descent, &unscaled_line_gap);

    const float ascent = glm::floor(unscaled_ascent * scale + ((unscaled_ascent > 0.0f) ? +1 : -1));
    const float descent = glm::floor(unscaled_descent * scale + ((unscaled_descent > 0.0f) ? +1 : -1));
    const float font_off_x = 0.f;
    const float font_off_y = glm::round(ascent);

    forrange(i, 0, codepoints.size())
    {
        const stbtt_packedchar& pc = packedChars[i];
        stbtt_aligned_quad q;
        float unused_x = 0.0f, unused_y = 0.0f;
        stbtt_GetPackedQuad(packedChars.data(), wantedWidth, wantedHeight, i, &unused_x, &unused_y, &q, 0);
        FontGlyph glyph;
        glyph.codePoint = codepoints[i];
        glyph.x0 = q.x0 + font_off_x;
        glyph.y0 = q.y0 + font_off_y;
        glyph.x1 = q.x1 + font_off_x;
        glyph.y1 = q.y1 + font_off_y;
        glyph.u0 = q.s0;
        glyph.v0 = q.t0;
        glyph.u1 = q.s1;
        glyph.v1 = q.t1;
        glyph.advance = pc.xadvance;
        PushGlyph(glyph);
    }

    // Create the texture
    FFontTexture = Rendering::FontTextureManager::Instance().AddNewFontTexture(parRes.FName, wantedWidth, wantedHeight, textureData);

    delete[] textureData;
}

const FontGlyph* Font::GetGlyph(u32 codePoint)
{
    auto itFind = FCodepointToGlyphMap.find(codePoint);
    if (itFind == FCodepointToGlyphMap.end())
        return nullptr;
    AssertRelease(itFind->second < FGlyphs.size());
    return &FGlyphs[itFind->second];
}

} // namespace UI
} // namespace ECSEngine
