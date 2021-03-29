#include "stdafx.h"

#include "VertexLayout.h"

#include "bgfx/bgfx.h"

namespace ECSEngine
{
namespace Rendering
{
bgfx::VertexLayout GetVertexLayout(const VertexLayoutHash& parVertexLayoutHash)
{
    bgfx::VertexLayout pcvDecl;
    pcvDecl.begin();
    if (parVertexLayoutHash.GetValue(VERTEX_LAYOUT_PARAMS::HAS_POSTION))
        pcvDecl.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float);

    if (parVertexLayoutHash.GetValue(VERTEX_LAYOUT_PARAMS::HAS_COLORS))
    {
        const u32 colorsNb = parVertexLayoutHash.GetColorsNb();
        switch (colorsNb)
        {
        case 1:
            pcvDecl.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true);
            break;
        case 2:
            pcvDecl.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true);
            pcvDecl.add(bgfx::Attrib::Color1, 4, bgfx::AttribType::Uint8, true);
            break;
        case 3:
            pcvDecl.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true);
            pcvDecl.add(bgfx::Attrib::Color1, 4, bgfx::AttribType::Uint8, true);
            pcvDecl.add(bgfx::Attrib::Color2, 4, bgfx::AttribType::Uint8, true);
            break;
        case 4:
            pcvDecl.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true);
            pcvDecl.add(bgfx::Attrib::Color1, 4, bgfx::AttribType::Uint8, true);
            pcvDecl.add(bgfx::Attrib::Color2, 4, bgfx::AttribType::Uint8, true);
            pcvDecl.add(bgfx::Attrib::Color3, 4, bgfx::AttribType::Uint8, true);
            break;
        default:
            AssertNotReached();
        }
    }

    if (parVertexLayoutHash.GetValue(VERTEX_LAYOUT_PARAMS::HAS_UVS))
    {
        const u32 uvsNb = parVertexLayoutHash.GetUVsNb();
        switch (uvsNb)
        {
        case 1:
            pcvDecl.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float);
            break;
        case 2:
            pcvDecl.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord1, 2, bgfx::AttribType::Float);
            break;
        case 3:
            pcvDecl.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord1, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord2, 2, bgfx::AttribType::Float);
            break;
        case 4:
            pcvDecl.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord1, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord2, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord3, 2, bgfx::AttribType::Float);
            break;
        case 5:
            pcvDecl.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord1, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord2, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord3, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord4, 2, bgfx::AttribType::Float);
            break;
        case 6:
            pcvDecl.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord1, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord2, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord3, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord4, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord5, 2, bgfx::AttribType::Float);
            break;
        case 7:
            pcvDecl.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord1, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord2, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord3, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord4, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord5, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord6, 2, bgfx::AttribType::Float);
            break;
        case 8:
            pcvDecl.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord1, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord2, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord3, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord4, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord5, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord6, 2, bgfx::AttribType::Float);
            pcvDecl.add(bgfx::Attrib::TexCoord7, 2, bgfx::AttribType::Float);
            break;
        default:
            AssertNotReached();
        }
    }

    if (parVertexLayoutHash.GetValue(VERTEX_LAYOUT_PARAMS::HAS_NORMALS))
        pcvDecl.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float);

    if (parVertexLayoutHash.GetValue(VERTEX_LAYOUT_PARAMS::HAS_TANGENTS))
        pcvDecl.add(bgfx::Attrib::Tangent, 3, bgfx::AttribType::Float);

    if (parVertexLayoutHash.GetValue(VERTEX_LAYOUT_PARAMS::HAS_BINORMALS))
        pcvDecl.add(bgfx::Attrib::Bitangent, 3, bgfx::AttribType::Float);

    pcvDecl.end();

    return pcvDecl;
}

} // namespace Rendering
} // namespace ECSEngine
