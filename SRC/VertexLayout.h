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

class IVertexLayout
{
public:
    virtual ~IVertexLayout() {}

    virtual bgfx::VertexLayout GetVertexLayout() const = 0;
};

class VextexPosition : public IVertexLayout
{
public:
    virtual bgfx::VertexLayout GetVertexLayout() const override;

    glm::vec3 FPosition;
};

class VextexPositionColor : public IVertexLayout
{
public:
    virtual bgfx::VertexLayout GetVertexLayout() const override;

    glm::vec3 FPosition;
    u32 FColor;
};

template<int N>
class VextexPositionColorN : public IVertexLayout
{
public:
    virtual bgfx::VertexLayout GetVertexLayout() const override;

    glm::vec3 FPosition;
    u32 FColor[N];
};

} // namespace Rendering
} // namespace ECSEngine

#include "VertextLayout.inl"