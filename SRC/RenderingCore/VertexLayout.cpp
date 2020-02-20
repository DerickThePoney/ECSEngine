#include "stdafx.h"

#include "VertexLayout.h"

#include "bgfx/bgfx.h"

namespace ECSEngine
{
namespace Rendering
{

bgfx::VertexLayout VextexPosition::GetVertexLayout() const
{
    bgfx::VertexLayout pcvDecl;
    pcvDecl.begin().add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float);

    return pcvDecl;
}

bgfx::VertexLayout VextexPositionColor::GetVertexLayout() const
{
    bgfx::VertexLayout pcvDecl;
    pcvDecl.begin().add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float);
    pcvDecl.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true).end();

    return pcvDecl;
}

} // namespace Rendering
} // namespace ECSEngine