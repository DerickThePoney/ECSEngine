#pragma once
#include "VectorTypes.h"

namespace ECSEngine
{
struct alignas(16) Matrix2x2f
{
public:
    Matrix2x2f() = default;
    Matrix2x2f(float parValue);
    Matrix2x2f(const vec2& A, const vec2& B);

private:
    float FValues[4] = { 0.f, 0.f, 0.f, 0.f };
};

using mat2 = Matrix2x2f;
}