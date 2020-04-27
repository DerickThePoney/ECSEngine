
#include "bgfx/bgfx.h"

namespace ECSEngine
{
namespace Rendering
{
template<int N>
VertexLayoutHash VertexPositionColorN<N>::LayoutHash = VertexLayoutHash(true, N, 0, false, false, false);
// template<int N>
// void VertexPositionColorN<N>::FillMeshFileHeader(MeshFileHeader& parMeshFileHeader)
//{
//    static_assert(N > 0 && N <= 4, "N doit être compris entre 1 et 4");
//    parMeshFileHeader.HasPositions = true;
//    parMeshFileHeader.HasColors = true;
//    parMeshFileHeader.NbColorChannels = N;
//    parMeshFileHeader.VertexSizeInOctet = sizeof(this);
//}

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

template<int N>
VertexLayoutHash VertexPositionUVNNormal<N>::LayoutHash = VertexLayoutHash(true, 0, N, true, false, false);
template<int N>
bgfx::VertexLayout VertexPositionUVNNormal<N>::GetVertexLayout() const
{
    static_assert(N > 0 && N <= 8, "N doit être compris entre 1 et 8");
    bgfx::VertexLayout pcvDecl;
    pcvDecl.begin();
    pcvDecl.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float);

    switch (N)
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
    }

    pcvDecl.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float);

    pcvDecl.end();

    return pcvDecl;
}

} // namespace Rendering
} // namespace ECSEngine
