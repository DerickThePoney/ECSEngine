#include "stdafx.h"

#include "VertexLayout.h"

//#include "MeshFileStreaming.h"
#include "bgfx/bgfx.h"

namespace ECSEngine
{
namespace Rendering
{
VertexLayoutHash VertexPosition::LayoutHash = VertexLayoutHash(true, 0, 0, false, false, false);

bgfx::VertexLayout VertexPosition::GetVertexLayout() const
{
    bgfx::VertexLayout pcvDecl;
    pcvDecl.begin().add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float);

    return pcvDecl;
}

} // namespace Rendering
} // namespace ECSEngine