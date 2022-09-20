#pragma once
#include "VectorTypes.h"
#include "MatrixTypes.h"

namespace ECSEngine
{
struct alignas(16) Matrix2x2f
{
    static constexpr u32 Size = 2;

    Matrix2x2f() = default;
    Matrix2x2f(float parValue);
    Matrix2x2f(const vec2& A, const vec2& B);

    const vec2 Row(u32 idx) const;
    const vec2 Column(u32 idx) const;

    void SetRow(u32 idx, const vec2& row);
    void SetColumn(u32 idx, const vec2& column);

    inline Matrix2x2f operator * (const Matrix2x2f& parA);
    inline Matrix2x2f operator + (const Matrix2x2f& parA);
    inline Matrix2x2f operator - (const Matrix2x2f& parA);

    inline void operator*=(const Matrix2x2f& parA);
    inline void operator+=(const Matrix2x2f& parA);
    inline void operator-=(const Matrix2x2f& parA);


    static Matrix2x2f Identity();


    float FValues[4] = { 0.f, 0.f, 0.f, 0.f };
};

inline Matrix2x2f operator*(const float parA, const Matrix2x2f& parB);

}