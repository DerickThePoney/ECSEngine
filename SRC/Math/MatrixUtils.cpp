#include "MatrixUtils.h"
#include "Matrix.h"
#include "Vector.h"
#include "VectorUtils.h"
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
    float f00 = parA.FValues[10] * parA.FValues[15] - parA.FValues[11] * parA.FValues[14];
    float f01 = parA.FValues[6] * parA.FValues[15] - parA.FValues[7] * parA.FValues[14];
    float f02 = parA.FValues[6] * parA.FValues[11] - parA.FValues[7] * parA.FValues[10];
    float f03 = parA.FValues[2] * parA.FValues[15] - parA.FValues[3] * parA.FValues[14];
    float f04 = parA.FValues[2] * parA.FValues[11] - parA.FValues[3] * parA.FValues[10];
    float f05 = parA.FValues[2] * parA.FValues[7] - parA.FValues[3] * parA.FValues[6];

    vec4 DetCof(+(parA.FValues[5] * f00 - parA.FValues[9] * f01 + parA.FValues[13] * f02), -(parA.FValues[1] * f00 - parA.FValues[9] * f03 + parA.FValues[13] * f04),
          +(parA.FValues[1] * f01 - parA.FValues[5] * f03 + parA.FValues[13] * f05), -(parA.FValues[1] * f02 - parA.FValues[5] * f04 + parA.FValues[9] * f05));

    return Dot(parA.Column(0), DetCof);
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
    float Coef00 = parA.FValues[10] * parA.FValues[15] - parA.FValues[11] * parA.FValues[14];
    float Coef02 = parA.FValues[9] * parA.FValues[15] - parA.FValues[11] * parA.FValues[13];
    float Coef03 = parA.FValues[9] * parA.FValues[14] - parA.FValues[10] * parA.FValues[13];

    float Coef04 = parA.FValues[6] * parA.FValues[15] - parA.FValues[7] * parA.FValues[14];
    float Coef06 = parA.FValues[5] * parA.FValues[15] - parA.FValues[7] * parA.FValues[13];
    float Coef07 = parA.FValues[5] * parA.FValues[14] - parA.FValues[6] * parA.FValues[13];

    float Coef08 = parA.FValues[6] * parA.FValues[11] - parA.FValues[7] * parA.FValues[10];
    float Coef10 = parA.FValues[5] * parA.FValues[11] - parA.FValues[7] * parA.FValues[9];
    float Coef11 = parA.FValues[5] * parA.FValues[10] - parA.FValues[6] * parA.FValues[9];

    float Coef12 = parA.FValues[2] * parA.FValues[15] - parA.FValues[3] * parA.FValues[14];
    float Coef14 = parA.FValues[1] * parA.FValues[15] - parA.FValues[3] * parA.FValues[13];
    float Coef15 = parA.FValues[1] * parA.FValues[14] - parA.FValues[2] * parA.FValues[13];

    float Coef16 = parA.FValues[2] * parA.FValues[11] - parA.FValues[3] * parA.FValues[10];
    float Coef18 = parA.FValues[1] * parA.FValues[11] - parA.FValues[3] * parA.FValues[9];
    float Coef19 = parA.FValues[1] * parA.FValues[10] - parA.FValues[2] * parA.FValues[9];

    float Coef20 = parA.FValues[2] * parA.FValues[7] - parA.FValues[3] * parA.FValues[6];
    float Coef22 = parA.FValues[1] * parA.FValues[7] - parA.FValues[3] * parA.FValues[5];
    float Coef23 = parA.FValues[1] * parA.FValues[6] - parA.FValues[2] * parA.FValues[5];

    vec4 Fac0(Coef00, Coef00, Coef02, Coef03);
    vec4 Fac1(Coef04, Coef04, Coef06, Coef07);
    vec4 Fac2(Coef08, Coef08, Coef10, Coef11);
    vec4 Fac3(Coef12, Coef12, Coef14, Coef15);
    vec4 Fac4(Coef16, Coef16, Coef18, Coef19);
    vec4 Fac5(Coef20, Coef20, Coef22, Coef23);

    vec4 Vec0(parA.FValues[1], parA.FValues[0], parA.FValues[0], parA.FValues[0]);
    vec4 Vec1(parA.FValues[5], parA.FValues[4], parA.FValues[4], parA.FValues[4]);
    vec4 Vec2(parA.FValues[9], parA.FValues[8], parA.FValues[8], parA.FValues[8]);
    vec4 Vec3(parA.FValues[13], parA.FValues[12], parA.FValues[12], parA.FValues[12]);

    vec4 Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
    vec4 Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
    vec4 Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
    vec4 Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

    vec4 SignA(+1, -1, +1, -1);
    vec4 SignB(-1, +1, -1, +1);
    mat4 Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA, Inv3 * SignB);

    vec4 Row0(Inverse.FValues[0], Inverse.FValues[1], Inverse.FValues[2], Inverse.FValues[3]);

    vec4 Dot0(parA.Column(0) * Row0);
    float Dot1 = (Dot0.x + Dot0.y) + (Dot0.z + Dot0.w);

    AlwaysCheckedAssert(Dot1 != 0.f);
    if (Dot1 == 0.f)
        return mat4();

    float ooDeter = 1.f / Dot1;

    return Inverse * ooDeter;
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
