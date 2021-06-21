#pragma once
#include "Common/RenderingHandles.h"

namespace ECSEngine
{
class Resource;
namespace Rendering
{
class VertexDataStream;
}

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
    bool visible = false;
};

struct FontGlyphInfo
{
    std::vector<FontGlyph> Glyphs;
    std::unordered_map<u32, u32> CodepointToGlyphMap;
};

class Font
{
public:
    ~Font();

    void InitFromResource(const Resource& parRes, const std::set<float>& parFontSizes);

    const Rendering::TextureHandle& FontTexture() const { return FFontTexture; }

    void PushGlyph(float parFontSize, FontGlyph parGlyph);

    const FontGlyph* GetGlyph(float parFontSize, u32 codePoint) const;

    // use font size
    void RasterizeText(const std::string& parText,
          const float parFontSize,
          const u32 parTextColor,
          const glm::vec2 startPosition,
          Rendering::VertexDataStream& outStream,
          std::vector<u32>& outIndices) const;
    // use font size
    glm::vec2 CalculateTextWidth(const std::string& parText, const float parFontSize) const;

    // void SetFontSize(const float parFontSize) { FFontSize = parFontSize; }

private:
    glm::vec2 CalculateTextWidth(const c8* parBegin, const c8* parEnd, const float parFontSize) const;
    void FillVerticesStream(const c8* parBegin,
          const c8* parEnd,
          const float parFontSize,
          const u32 parTextColor,
          const glm::vec2 parStartPosition,
          Rendering::VertexDataStream& outStream,
          std::vector<u32>& outIndices) const;

private:
    std::set<float> FFontSizes;

    std::unordered_map<float, FontGlyphInfo> FFontSizeToGlyphInfoMap;
    Rendering::TextureHandle FFontTexture;

    u32 FFallbackGlyphIndex = -1;
};
} // namespace UI
} // namespace ECSEngine