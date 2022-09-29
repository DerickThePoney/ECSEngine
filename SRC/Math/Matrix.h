#pragma once
#include "MatrixTypes.h"
#include "VectorTypes.h"

namespace ECSEngine
{
/****************************
 * Matrix2x2f
 ****************************/
struct alignas(4) Matrix2x2f
{
    static constexpr u32 Size = 2;

    Matrix2x2f() = default;
    explicit Matrix2x2f(float parValue);
    explicit Matrix2x2f(const vec2& A, const vec2& B);

    const vec2 Row(u32 idx) const;
    const vec2 Column(u32 idx) const;

    void SetRow(u32 idx, const vec2& row);
    void SetColumn(u32 idx, const vec2& column);

    inline Matrix2x2f operator*(const float parA)
    {
        Matrix2x2f result;
        result.FValues[0] = parA * FValues[0];
        result.FValues[1] = parA * FValues[1];
        result.FValues[2] = parA * FValues[2];
        result.FValues[3] = parA * FValues[3];
        return result;
    }
    Matrix2x2f operator*(const Matrix2x2f& parA) const;
    vec2 operator*(const vec2& parA) const;
    Matrix2x2f operator+(const Matrix2x2f& parA) const;
    Matrix2x2f operator-(const Matrix2x2f& parA) const;

    inline void operator-=(const Matrix2x2f& parA)
    {
        FValues[0] -= parA.FValues[0];
        FValues[1] -= parA.FValues[1];
        FValues[2] -= parA.FValues[2];
        FValues[3] -= parA.FValues[3];
    }

    inline void operator+=(const Matrix2x2f& parA)
    {
        FValues[0] += parA.FValues[0];
        FValues[1] += parA.FValues[1];
        FValues[2] += parA.FValues[2];
        FValues[3] += parA.FValues[3];
    }

    inline void operator*=(const Matrix2x2f& parA)
    {
        FValues[0] = FValues[0] * parA.FValues[0] + FValues[1] * parA.FValues[2];
        FValues[1] = FValues[0] * parA.FValues[1] + FValues[1] * parA.FValues[3];
        FValues[2] = FValues[2] * parA.FValues[0] + FValues[3] * parA.FValues[2];
        FValues[3] = FValues[2] * parA.FValues[1] + FValues[3] * parA.FValues[3];
    }

    inline Matrix2x2f operator*=(const float parA)
    {
        FValues[0] = parA * FValues[0];
        FValues[1] = parA * FValues[1];
        FValues[2] = parA * FValues[2];
        FValues[3] = parA * FValues[3];
    }

    static Matrix2x2f Identity();

    float FValues[4] = { 0.f, 0.f, 0.f, 0.f };
};

inline Matrix2x2f operator*(const float parA, const Matrix2x2f& parB)
{
    Matrix2x2f result;
    result.FValues[0] = parA * parB.FValues[0];
    result.FValues[1] = parA * parB.FValues[1];
    result.FValues[2] = parA * parB.FValues[2];
    result.FValues[3] = parA * parB.FValues[3];
    return result;
}

inline Matrix2x2f operator*(const Matrix2x2f& parA, const Matrix2x2f& parB)
{
    Matrix2x2f result = parA;
    result *= parB;
    return result;
}

/****************************
 * Matrix3x3f
 ****************************/

struct alignas(4) Matrix3x3f
{
    static constexpr u32 Size = 3;

    Matrix3x3f() = default;
    explicit Matrix3x3f(float parValue);
    explicit Matrix3x3f(const vec3& A, const vec3& B, const vec3& C);

    const vec3 Row(u32 idx) const;
    const vec3 Column(u32 idx) const;

    void SetRow(u32 idx, const vec3& row);
    void SetColumn(u32 idx, const vec3& column);

    inline Matrix3x3f operator*(const float parA)
    {
        Matrix3x3f result;
        result.FValues[0] = parA * FValues[0];
        result.FValues[1] = parA * FValues[1];
        result.FValues[2] = parA * FValues[2];
        result.FValues[3] = parA * FValues[3];
        result.FValues[4] = parA * FValues[4];
        result.FValues[5] = parA * FValues[5];
        result.FValues[6] = parA * FValues[6];
        result.FValues[7] = parA * FValues[7];
        result.FValues[8] = parA * FValues[8];
        return result;
    }
    Matrix3x3f operator*(const Matrix3x3f& parA) const;
    vec3 operator*(const vec3& parA) const;
    Matrix3x3f operator+(const Matrix3x3f& parA) const;
    Matrix3x3f operator-(const Matrix3x3f& parA) const;

    inline void operator-=(const Matrix3x3f& parA)
    {
        FValues[0] -= parA.FValues[0];
        FValues[1] -= parA.FValues[1];
        FValues[2] -= parA.FValues[2];
        FValues[3] -= parA.FValues[3];
        FValues[4] -= parA.FValues[4];
        FValues[5] -= parA.FValues[5];
        FValues[6] -= parA.FValues[6];
        FValues[7] -= parA.FValues[7];
        FValues[8] -= parA.FValues[8];
    }

    inline void operator+=(const Matrix3x3f& parA)
    {
        FValues[0] += parA.FValues[0];
        FValues[1] += parA.FValues[1];
        FValues[2] += parA.FValues[2];
        FValues[3] += parA.FValues[3];
        FValues[4] += parA.FValues[4];
        FValues[5] += parA.FValues[5];
        FValues[6] += parA.FValues[6];
        FValues[7] += parA.FValues[7];
        FValues[8] += parA.FValues[8];
    }

    inline void operator*=(const Matrix3x3f& parA)
    {
        FValues[0] = FValues[0] * parA.FValues[0] + FValues[1] * parA.FValues[3] + FValues[2] * parA.FValues[6];
        FValues[1] = FValues[0] * parA.FValues[1] + FValues[1] * parA.FValues[4] + FValues[2] * parA.FValues[7];
        FValues[2] = FValues[0] * parA.FValues[2] + FValues[1] * parA.FValues[5] + FValues[2] * parA.FValues[8];
        FValues[3] = FValues[3] * parA.FValues[0] + FValues[4] * parA.FValues[3] + FValues[5] * parA.FValues[6];
        FValues[4] = FValues[3] * parA.FValues[1] + FValues[4] * parA.FValues[4] + FValues[5] * parA.FValues[7];
        FValues[5] = FValues[3] * parA.FValues[2] + FValues[4] * parA.FValues[5] + FValues[5] * parA.FValues[8];
        FValues[6] = FValues[6] * parA.FValues[0] + FValues[7] * parA.FValues[3] + FValues[8] * parA.FValues[6];
        FValues[7] = FValues[6] * parA.FValues[1] + FValues[7] * parA.FValues[4] + FValues[8] * parA.FValues[7];
        FValues[8] = FValues[6] * parA.FValues[2] + FValues[7] * parA.FValues[5] + FValues[8] * parA.FValues[8];
    }

    inline Matrix3x3f operator*=(const float parA)
    {
        FValues[0] = parA * FValues[0];
        FValues[1] = parA * FValues[1];
        FValues[2] = parA * FValues[2];
        FValues[3] = parA * FValues[3];
        FValues[4] = parA * FValues[4];
        FValues[5] = parA * FValues[5];
        FValues[6] = parA * FValues[6];
        FValues[7] = parA * FValues[7];
        FValues[8] = parA * FValues[8];
    }

    static Matrix3x3f Identity();

    float FValues[9] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
};

inline Matrix3x3f operator*(const float parA, const Matrix3x3f& parB)
{
    Matrix3x3f result;
    result.FValues[0] = parA * parB.FValues[0];
    result.FValues[1] = parA * parB.FValues[1];
    result.FValues[2] = parA * parB.FValues[2];
    result.FValues[3] = parA * parB.FValues[3];
    result.FValues[4] = parA * parB.FValues[4];
    result.FValues[5] = parA * parB.FValues[5];
    result.FValues[6] = parA * parB.FValues[6];
    result.FValues[7] = parA * parB.FValues[7];
    result.FValues[8] = parA * parB.FValues[8];
    return result;
}

inline Matrix3x3f operator*(const Matrix3x3f& parA, const Matrix3x3f& parB)
{
    Matrix3x3f result = parA;
    result *= parB;
    return result;
}

/****************************
 * Matrix4x4f
 ****************************/

struct alignas(4) Matrix4x4f
{
    static constexpr u32 Size = 4;

    Matrix4x4f() = default;
    explicit Matrix4x4f(float parValue);
    explicit Matrix4x4f(const vec4& A, const vec4& B, const vec4& C, const vec4& D);

    const vec4 Row(u32 idx) const;
    const vec4 Column(u32 idx) const;

    void SetRow(u32 idx, const vec4& row);
    void SetColumn(u32 idx, const vec4& column);

    inline Matrix4x4f operator*(const float parA) const
    {
        Matrix4x4f result;
        for (u32 i = 0; i < Matrix4x4f::Size * Matrix4x4f::Size; ++i)
            result.FValues[i] = parA * FValues[i];
        return result;
    }
    Matrix4x4f operator*(const Matrix4x4f& parA) const;
    vec4 operator*(const vec4& parA) const;
    Matrix4x4f operator+(const Matrix4x4f& parA) const;
    Matrix4x4f operator-(const Matrix4x4f& parA) const;

    inline void operator-=(const Matrix4x4f& parA)
    {
        for (u32 i = 0; i < Matrix4x4f::Size * Matrix4x4f::Size; ++i)
            FValues[i] -= parA.FValues[i];
    }

    inline void operator+=(const Matrix4x4f& parA)
    {
        for (u32 i = 0; i < Matrix4x4f::Size * Matrix4x4f::Size; ++i)
            FValues[i] += parA.FValues[i];
    }

    inline void operator*=(const Matrix4x4f& parA)
    {
        for (u32 r = 0; r < Matrix4x4f::Size; ++r)
        {
            for (u32 c = 0; c < Matrix4x4f::Size; ++c)
            {
                FValues[r * Size + c] = FValues[r * Size] * parA.FValues[c] + FValues[r * Size + 1] * parA.FValues[Size + c] + FValues[r * Size + 2] * parA.FValues[2 * Size + c] +
                      FValues[r * Size + 3] * parA.FValues[3 * Size + c];
            }
        }
    }

    inline Matrix4x4f operator*=(const float parA)
    {
        for (u32 i = 0; i < Matrix4x4f::Size * Matrix4x4f::Size; ++i)
            FValues[i] = parA * FValues[i];
    }

    static Matrix4x4f Identity();

    float FValues[16] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
};

inline Matrix4x4f operator*(const float parA, const Matrix4x4f& parB)
{
    Matrix4x4f result;
    for (u32 i = 0; i < Matrix4x4f::Size * Matrix4x4f::Size; ++i)
        result.FValues[i] = parA * parB.FValues[i];
    return result;
}

//inline Matrix4x4f operator*(const Matrix4x4f& parA, const Matrix4x4f& parB)
//{
//    Matrix4x4f result = parA;
//    result *= parB;
//    return result;
//}

//inline vec4 operator*(const Matrix4x4f& parA, const vec4& parB)
//{
//    vec4 result = parA.operator *(parB);
//    return result;
//}

} // namespace ECSEngine