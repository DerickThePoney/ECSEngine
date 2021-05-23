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

class Font
{
public:
    void InitFromResource(const Resource& parRes);

    float FontSize() const { return FFontSize; }
    const Rendering::TextureHandle& FontTexture() const { return FFontTexture; }

    void PushGlyph(FontGlyph parGlyph)
    {
        AssertRelease(FCodepointToGlyphMap.find(parGlyph.codePoint) == FCodepointToGlyphMap.end());
        FCodepointToGlyphMap[parGlyph.codePoint] = FGlyphs.size();
        FGlyphs.push_back(parGlyph);
    }

    const FontGlyph* GetGlyph(u32 codePoint);

    void RasterizeText(const std::string& parText, const glm::vec2 startPosition, Rendering::VertexDataStream& outStream, std::vector<u32>& outIndices);

private:
    glm::vec2 CalculateTextWidth(const c8* parBegin, const c8* parEnd);
    void FillVerticesStream(const c8* parBegin,
          const c8* parEnd,
          const glm::vec2 parStartPosition,
          const glm::vec2 parTextSize,
          Rendering::VertexDataStream& outStream,
          std::vector<u32>& outIndices);

private:
    float FFontSize = 30.f;

    std::vector<FontGlyph> FGlyphs;
    std::unordered_map<u32, u32> FCodepointToGlyphMap;
    Rendering::TextureHandle FFontTexture;

    u32 FFallbackGlyphIndex = -1;
};
} // namespace UI
} // namespace ECSEngine