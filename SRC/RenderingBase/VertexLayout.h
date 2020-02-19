#pragma once

namespace bgfx
{
struct VertexLayout;
}

namespace ECSEngine
{
namespace Rendering
{
template<typename T>
bgfx::VertexLayout GetVertexLayout(const T& parVertexLayout)
{
    return parVertexLayout.GetVertexLayout();
}

struct VextexPosition
{
public:
    bgfx::VertexLayout GetVertexLayout() const;

    glm::vec3 FPosition;
};
static_assert(sizeof(VextexPosition) == 12);

struct VextexPositionColor
{
public:
    bgfx::VertexLayout GetVertexLayout() const;

    glm::vec3 FPosition;
    u32 FColor;
};
static_assert(sizeof(VextexPositionColor) == 16);

template<int N>
struct VertexPositionColorN
{
public:
    bgfx::VertexLayout GetVertexLayout() const;

    glm::vec3 FPosition;
    u32 FColor[N];
};

static_assert(sizeof(VertexPositionColorN<1>) == 16);
static_assert(sizeof(VertexPositionColorN<2>) == 20);
static_assert(sizeof(VertexPositionColorN<3>) == 24);
static_assert(sizeof(VertexPositionColorN<4>) == 28);
} // namespace Rendering
} // namespace ECSEngine

#include "VertextLayout.inl"