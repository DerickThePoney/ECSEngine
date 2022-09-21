#include "MatrixUtils.h"
#include "Matrix.h"
#include "Vector.h"
#include "Common/Assertions.h"

namespace ECSEngine
{

mat2 Mul(const mat2& parA, const mat2& parB)
{
    mat2 result;
    result.FValues[0] = parA.FValues[0] * parB.FValues[0] + parA.FValues[1] * parB.FValues[2];
    result.FValues[1] = parA.FValues[0] * parB.FValues[1] + parA.FValues[1] * parB.FValues[3];
    result.FValues[2] = parA.FValues[2] * parB.FValues[0] + parA.FValues[3] * parB.FValues[2];
    result.FValues[3] = parA.FValues[2] * parB.FValues[1] + parA.FValues[3] * parB.FValues[3];
    return result;
}

mat3 Mul(const mat3& parA, const mat3& parB)
{
    mat3 result;
    result.FValues[0] = parA.FValues[0] * parB.FValues[0] + parA.FValues[1] * parB.FValues[3] + parA.FValues[2] * parB.FValues[6];
    result.FValues[1] = parA.FValues[0] * parB.FValues[1] + parA.FValues[1] * parB.FValues[4] + parA.FValues[2] * parB.FValues[7];
    result.FValues[2] = parA.FValues[0] * parB.FValues[2] + parA.FValues[1] * parB.FValues[5] + parA.FValues[2] * parB.FValues[8];
    result.FValues[3] = parA.FValues[3] * parB.FValues[0] + parA.FValues[4] * parB.FValues[3] + parA.FValues[5] * parB.FValues[6];
    result.FValues[4] = parA.FValues[3] * parB.FValues[1] + parA.FValues[4] * parB.FValues[4] + parA.FValues[5] * parB.FValues[7];
    result.FValues[5] = parA.FValues[3] * parB.FValues[2] + parA.FValues[4] * parB.FValues[5] + parA.FValues[5] * parB.FValues[8];
    result.FValues[6] = parA.FValues[6] * parB.FValues[0] + parA.FValues[7] * parB.FValues[3] + parA.FValues[8] * parB.FValues[6];
    result.FValues[7] = parA.FValues[6] * parB.FValues[1] + parA.FValues[7] * parB.FValues[4] + parA.FValues[8] * parB.FValues[7];
    result.FValues[8] = parA.FValues[6] * parB.FValues[2] + parA.FValues[7] * parB.FValues[5] + parA.FValues[8] * parB.FValues[8];
    return result;
}

mat4 Mul(const mat4& parA, const mat4& parB)
{
    mat4 result;
    for (u32 r = 0; r < Matrix4x4f::Size; ++r)
    {
        for (u32 c = 0; c < Matrix4x4f::Size; ++c)
        {
            result.FValues[r * mat4::Size + c] = parA.FValues[r * mat4::Size] * parB.FValues[c] + parA.FValues[r * mat4::Size + 1] * parB.FValues[mat4::Size + c] +
                  parA.FValues[r * mat4::Size + 2] * parB.FValues[2 * mat4::Size + c] + parA.FValues[r * mat4::Size + 3] * parB.FValues[3 * mat4::Size + c];
        }
    }
    return result;
}

mat2 Add(const mat2& parA, const mat2& parB)
{
    mat2 result;
    result.FValues[0] = parA.FValues[0] + parB.FValues[0];
    result.FValues[1] = parA.FValues[1] + parB.FValues[1];
    result.FValues[2] = parA.FValues[2] + parB.FValues[2];
    result.FValues[3] = parA.FValues[3] + parB.FValues[3];
    return result;
}

mat3 Add(const mat3& parA, const mat3& parB)
{
    mat3 result;
    result.FValues[0] = parA.FValues[0] + parA.FValues[0];
    result.FValues[1] = parA.FValues[1] + parA.FValues[1];
    result.FValues[2] = parA.FValues[2] + parA.FValues[2];
    result.FValues[3] = parA.FValues[3] + parA.FValues[3];
    result.FValues[4] = parA.FValues[4] + parA.FValues[4];
    result.FValues[5] = parA.FValues[5] + parA.FValues[5];
    result.FValues[6] = parA.FValues[6] + parA.FValues[6];
    result.FValues[7] = parA.FValues[7] + parA.FValues[7];
    result.FValues[8] = parA.FValues[8] + parA.FValues[8];
    return result;
}

mat4 Add(const mat4& parA, const mat4& parB)
{
    mat4 result;
    for (u32 i = 0; i < Matrix4x4f::Size * Matrix4x4f::Size; ++i)
        result.FValues[i] = parA.FValues[i] + parB.FValues[i];
    return result;
}

mat2 Sub(const mat2& parA, const mat2& parB)
{
    mat2 result;
    result.FValues[0] = parA.FValues[0] - parB.FValues[0];
    result.FValues[1] = parA.FValues[1] - parB.FValues[1];
    result.FValues[2] = parA.FValues[2] - parB.FValues[2];
    result.FValues[3] = parA.FValues[3] - parB.FValues[3];
    return result;
}

mat3 Sub(const mat3& parA, const mat3& parB)
{
    mat3 result;
    result.FValues[0] = parA.FValues[0] - parA.FValues[0];
    result.FValues[1] = parA.FValues[1] - parA.FValues[1];
    result.FValues[2] = parA.FValues[2] - parA.FValues[2];
    result.FValues[3] = parA.FValues[3] - parA.FValues[3];
    result.FValues[4] = parA.FValues[4] - parA.FValues[4];
    result.FValues[5] = parA.FValues[5] - parA.FValues[5];
    result.FValues[6] = parA.FValues[6] - parA.FValues[6];
    result.FValues[7] = parA.FValues[7] - parA.FValues[7];
    result.FValues[8] = parA.FValues[8] - parA.FValues[8];
    return result;
}

mat4 Sub(const mat4& parA, const mat4& parB)
{
    mat4 result;
    for (u32 i = 0; i < Matrix4x4f::Size * Matrix4x4f::Size; ++i)
        result.FValues[i] = parA.FValues[i] - parB.FValues[i];
    return result;
}

float Determinant(const mat2& parA)
{
    return parA.FValues[0] * parA.FValues[3] - parA.FValues[1] * parA.FValues[2];
}

float Determinant(const mat3& parA)
{
    return (+parA.FValues[0] * (parA.FValues[4] * parA.FValues[8] - parA.FValues[7] * parA.FValues[5]) -
          parA.FValues[3] * (parA.FValues[1] * parA.FValues[8] - parA.FValues[7] * parA.FValues[2]) +
          parA.FValues[6] * (parA.FValues[1] * parA.FValues[5] - parA.FValues[4] * parA.FValues[2]));
}

float Determinant(const mat4& parA)
{
    return 0.f;
}

mat2 Invert(const mat2& parA)
{
    mat2 result;

    const float deter = Determinant(parA);
    AlwaysCheckedAssert(deter != 0.f);
    if (deter == 0.f) // TODO relax condition a bit
        return result;

    result.FValues[0] = parA.FValues[3];
    result.FValues[1] = -parA.FValues[1];
    result.FValues[2] = -parA.FValues[2];
    result.FValues[3] = parA.FValues[0];
    return (1.f / deter) * result;
}

mat3 Invert(const mat3& parA)
{
    mat3 result;
    const float deter = Determinant(parA);
    AlwaysCheckedAssert(deter != 0.f);
    if (deter == 0.f)
        return result;

    const float ooDeter = 1.f / deter;          

    result.FValues[0] = +(parA.FValues[4] * parA.FValues[8] - parA.FValues[7] * parA.FValues[5]) * ooDeter;
    result.FValues[3] = -(parA.FValues[3] * parA.FValues[8] - parA.FValues[6] * parA.FValues[5]) * ooDeter;
    result.FValues[6] = +(parA.FValues[3] * parA.FValues[7] - parA.FValues[6] * parA.FValues[4]) * ooDeter;
    result.FValues[1] = -(parA.FValues[1] * parA.FValues[8] - parA.FValues[7] * parA.FValues[2]) * ooDeter;
    result.FValues[4] = +(parA.FValues[0] * parA.FValues[8] - parA.FValues[6] * parA.FValues[2]) * ooDeter;
    result.FValues[7] = -(parA.FValues[0] * parA.FValues[7] - parA.FValues[6] * parA.FValues[1]) * ooDeter;
    result.FValues[2] = +(parA.FValues[1] * parA.FValues[5] - parA.FValues[4] * parA.FValues[2]) * ooDeter;
    result.FValues[5] = -(parA.FValues[0] * parA.FValues[5] - parA.FValues[3] * parA.FValues[2]) * ooDeter;
    result.FValues[8] = +(parA.FValues[0] * parA.FValues[4] - parA.FValues[3] * parA.FValues[1]) * ooDeter;

    return result;
}

mat4 Invert(const mat4& parA)
{
    return mat4();
}

mat2 Transpose(const mat2& parA)
{
    return mat2(parA.Row(0), parA.Row(1));
}

mat3 Transpose(const mat3& parA)
{
    return mat3(parA.Row(0), parA.Row(1), parA.Row(2));
}

mat4 Transpose(const mat4& parA)
{
    return mat4(parA.Row(0), parA.Row(1), parA.Row(2), parA.Row(3));
}

}
