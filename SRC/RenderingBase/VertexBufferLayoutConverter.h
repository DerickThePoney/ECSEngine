#pragma once
#include "VertexBuffer.h"

namespace ECSEngine
{
namespace Rendering
{
template<typename FromLayout, typename ToLayout>
VertexBuffer<ToLayout>* ConvertBufferToNewLayout(VertexBuffer<FromLayout>* parSrc, bool parClearOldVertexBuffer)
{
}
} // namespace Rendering
} // namespace ECSEngine