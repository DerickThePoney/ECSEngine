
#include "bgfx/bgfx.h"

namespace ECSEngine
{
namespace Rendering
{

template<int N>
bgfx::VertexLayout VertexPositionColorN<N>::GetVertexLayout() const
{
    static_assert(N > 0 && N <= 4, "N doit être compris entre 1 et 4");
    bgfx::VertexLayout pcvDecl;
    pcvDecl.begin();
    pcvDecl.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float);

    switch (N)
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
    }

    pcvDecl.end();

    return pcvDecl;
}

} // namespace Rendering
} // namespace ECSEngine
