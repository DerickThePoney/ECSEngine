#pragma once
#include "Common/MeshStreamingData.h"

namespace bgfx
{
struct VertexLayout;
}
namespace ECSEngine
{
namespace Rendering
{
bgfx::VertexLayout GetVertexLayout(const VertexLayoutHash& parVertexLayoutHash);

} // namespace Rendering
} // namespace ECSEngine