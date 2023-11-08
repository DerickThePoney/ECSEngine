#include "MatrixUtils.h"

#include "Common/Assertions.h"
#include "MathFunctions.h"
#include "Matrix.h"
#include "Vector.h"
#include "VectorUtils.h"

#include <limits>

namespace ECSEngine
{

mat2 Mul(const mat2& parA, const mat2& parB)
{
    mat2 result;
    result.FValues[0] = parA.FValues[0] * parB.FValues[0] + parA.FValues[2] * parB.FValues[1];
    result.FValues[1] = parA.FValues[1] * parB.FValues[0] + parA.FValues[3] * parB.FValues[1];
    result.FValues[2] = parA.FValues[0] * parB.FValues[2] + parA.FValues[2] * parB.FValues[3];
    result.FValues[3] = parA.FValues[1] * parB.FValues[1] + parA.FValues[3] * parB.FValues[3];
    return result;
}

mat3 Mul(const mat3& parA, const mat3& parB)
{
    mat3 result;
    for (u32 c = 0; c < mat3::Size; ++c)
    {
        for (u32 r = 0; r < mat3::Size; ++r)
        {
            result.FValues[c * mat3::Size + r] = parA.FValues[r] * parB.FValues[c * mat3::Size] + parA.FValues[r + mat3::Size] * parB.FValues[c * mat3::Size + 1] +
                  parA.FValues[2 * mat3::Size + r] * parB.FValues[c * mat3::Size + 2];
        }
    }
    return result;
}

mat4 Mul(const mat4& parA, const mat4& parB)
{
    mat4 result;

    for (u32 c = 0; c < Matrix4x4f::Size; ++c)
    {
        for (u32 r = 0; r < Matrix4x4f::Size; ++r)
        {
            result.FValues[c * mat4::Size + r] = parA.FValues[r] * parB.FValues[c * mat4::Size] + parA.FValues[r + mat4::Size] * parB.FValues[c * mat4::Size + 1] +
                  parA.FValues[2 * mat4::Size + r] * parB.FValues[c * mat4::Size + 2] + parA.FValues[3 * mat4::Size + r] * parB.FValues[c * mat4::Size + 3];
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
          parA.FValues[1] * (parA.FValues[3] * parA.FValues[8] - parA.FValues[5] * parA.FValues[6]) +
          parA.FValues[2] * (parA.FValues[3] * parA.FValues[7] - parA.FValues[4] * parA.FValues[6]));
}

float Determinant(const mat4& parA)
{
    float f00 = parA.FValues[10] * parA.FValues[15] - parA.FValues[14] * parA.FValues[11];
    float f01 = parA.FValues[9] * parA.FValues[15] - parA.FValues[13] * parA.FValues[11];
    float f02 = parA.FValues[9] * parA.FValues[14] - parA.FValues[13] * parA.FValues[10];
    float f03 = parA.FValues[8] * parA.FValues[15] - parA.FValues[12] * parA.FValues[11];
    float f04 = parA.FValues[8] * parA.FValues[14] - parA.FValues[12] * parA.FValues[10];
    float f05 = parA.FValues[8] * parA.FValues[13] - parA.FValues[12] * parA.FValues[9];

    vec4 DetCof(+(parA.FValues[5] * f00 - parA.FValues[6] * f01 + parA.FValues[7] * f02), -(parA.FValues[4] * f00 - parA.FValues[6] * f03 + parA.FValues[7] * f04),
          +(parA.FValues[4] * f01 - parA.FValues[5] * f03 + parA.FValues[7] * f05), -(parA.FValues[4] * f02 - parA.FValues[5] * f04 + parA.FValues[6] * f05));

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

    result.FValues[0] = +(parA.FValues[4] * parA.FValues[8] - parA.FValues[5] * parA.FValues[7]) * ooDeter;
    result.FValues[1] = -(parA.FValues[1] * parA.FValues[8] - parA.FValues[2] * parA.FValues[7]) * ooDeter;
    result.FValues[2] = +(parA.FValues[1] * parA.FValues[5] - parA.FValues[2] * parA.FValues[4]) * ooDeter;
    result.FValues[3] = -(parA.FValues[3] * parA.FValues[8] - parA.FValues[5] * parA.FValues[6]) * ooDeter;
    result.FValues[4] = +(parA.FValues[0] * parA.FValues[8] - parA.FValues[2] * parA.FValues[6]) * ooDeter;
    result.FValues[5] = -(parA.FValues[0] * parA.FValues[5] - parA.FValues[2] * parA.FValues[3]) * ooDeter;
    result.FValues[6] = +(parA.FValues[3] * parA.FValues[7] - parA.FValues[4] * parA.FValues[6]) * ooDeter;
    result.FValues[7] = -(parA.FValues[0] * parA.FValues[7] - parA.FValues[1] * parA.FValues[6]) * ooDeter;
    result.FValues[8] = +(parA.FValues[0] * parA.FValues[4] - parA.FValues[1] * parA.FValues[3]) * ooDeter;

    return result;
}

mat4 Invert(const mat4& parA)
{
    float Coef00 = parA.FValues[10] * parA.FValues[15] - parA.FValues[14] * parA.FValues[11];
    float Coef02 = parA.FValues[6] * parA.FValues[15] - parA.FValues[14] * parA.FValues[7];
    float Coef03 = parA.FValues[6] * parA.FValues[11] - parA.FValues[10] * parA.FValues[7];

    float Coef04 = parA.FValues[9] * parA.FValues[15] - parA.FValues[13] * parA.FValues[11];
    float Coef06 = parA.FValues[5] * parA.FValues[15] - parA.FValues[13] * parA.FValues[7];
    float Coef07 = parA.FValues[5] * parA.FValues[11] - parA.FValues[9] * parA.FValues[7];

    float Coef08 = parA.FValues[9] * parA.FValues[14] - parA.FValues[13] * parA.FValues[10];
    float Coef10 = parA.FValues[5] * parA.FValues[14] - parA.FValues[13] * parA.FValues[6];
    float Coef11 = parA.FValues[5] * parA.FValues[10] - parA.FValues[9] * parA.FValues[6];

    float Coef12 = parA.FValues[8] * parA.FValues[15] - parA.FValues[12] * parA.FValues[11];
    float Coef14 = parA.FValues[4] * parA.FValues[15] - parA.FValues[12] * parA.FValues[7];
    float Coef15 = parA.FValues[4] * parA.FValues[11] - parA.FValues[8] * parA.FValues[7];

    float Coef16 = parA.FValues[8] * parA.FValues[14] - parA.FValues[12] * parA.FValues[10];
    float Coef18 = parA.FValues[4] * parA.FValues[14] - parA.FValues[12] * parA.FValues[6];
    float Coef19 = parA.FValues[4] * parA.FValues[10] - parA.FValues[8] * parA.FValues[6];

    float Coef20 = parA.FValues[8] * parA.FValues[13] - parA.FValues[12] * parA.FValues[9];
    float Coef22 = parA.FValues[4] * parA.FValues[13] - parA.FValues[12] * parA.FValues[5];
    float Coef23 = parA.FValues[4] * parA.FValues[9] - parA.FValues[8] * parA.FValues[5];

    vec4 Fac0(Coef00, Coef00, Coef02, Coef03);
    vec4 Fac1(Coef04, Coef04, Coef06, Coef07);
    vec4 Fac2(Coef08, Coef08, Coef10, Coef11);
    vec4 Fac3(Coef12, Coef12, Coef14, Coef15);
    vec4 Fac4(Coef16, Coef16, Coef18, Coef19);
    vec4 Fac5(Coef20, Coef20, Coef22, Coef23);

    vec4 Vec0(parA.FValues[4], parA.FValues[0], parA.FValues[0], parA.FValues[0]);
    vec4 Vec1(parA.FValues[5], parA.FValues[1], parA.FValues[1], parA.FValues[1]);
    vec4 Vec2(parA.FValues[6], parA.FValues[2], parA.FValues[2], parA.FValues[2]);
    vec4 Vec3(parA.FValues[7], parA.FValues[3], parA.FValues[3], parA.FValues[3]);

    vec4 Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
    vec4 Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
    vec4 Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
    vec4 Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

    vec4 SignA(+1, -1, +1, -1);
    vec4 SignB(-1, +1, -1, +1);
    mat4 Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA, Inv3 * SignB);

    vec4 Row0(Inverse.FValues[0], Inverse.FValues[4], Inverse.FValues[8], Inverse.FValues[12]);

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

mat4 Translation(const vec3& parT)
{
    mat4 t = mat4::Identity();
    t.SetColumn(3, vec4(parT, 1.f));
    return t;
}

mat4 Rotation(float angle, const vec3& v)
{
    const float a = angle;
    const float c = Cos(a);
    const float s = Sin(a);

    vec3 axis(Normalize(v));
    vec3 temp((1.f - c) * axis);

    mat4 rotate = mat4::Identity();
    rotate.FValues[0] = c + temp.x * axis.x;
    rotate.FValues[1] = temp.x * axis.y + s * axis.z;
    rotate.FValues[2] = temp.x * axis.z - s * axis.y;

    rotate.FValues[4] = temp.y * axis.x - s * axis.z;
    rotate.FValues[5] = c + temp.y * axis.y;
    rotate.FValues[6] = temp.y * axis.z + s * axis.x;

    rotate.FValues[8] = temp.z * axis.x + s * axis.y;
    rotate.FValues[9] = temp.z * axis.y - s * axis.x;
    rotate.FValues[10] = c + temp.z * axis.z;

    return rotate;
}

mat4 Perspective(float fovy, float aspect, float zNear, float zFar)
{
    AlwaysCheckedAssert(fabs(aspect - std::numeric_limits<float>::epsilon()) > 0.f);

    const float tanHalfFovy = tan(0.5f * fovy);

    mat4 Result;
    Result.FValues[0] = 1.f / (aspect * tanHalfFovy);
    Result.FValues[5] = 1.f / (tanHalfFovy);
    Result.FValues[10] = (zFar + zNear) / (zFar - zNear);
    Result.FValues[11] = 1.f;
    Result.FValues[14] = -(2.f * zFar * zNear) / (zFar - zNear);
    return Result;
}

mat4 Orthographic(float left, float right, float bottom, float top)
{
    mat4 Result = mat4::Identity();
    Result.FValues[0] = 2.f / (right - left);
    Result.FValues[5] = 2.f / (top - bottom);
    Result.FValues[10] = -1.f;
    Result.FValues[12] = -(right + left) / (right - left);
    Result.FValues[13] = -(top + bottom) / (top - bottom);
    return Result;
}

mat4 EulerAnglesXYZ(const float X, const float Y, const float Z)
{
    float c1 = cosf(-X);
    float c2 = cosf(-Y);
    float c3 = cosf(-Z);
    float s1 = sinf(-X);
    float s2 = sinf(-Y);
    float s3 = sinf(-Z);

    mat4 Result;
    Result.FValues[0] = c2 * c3;
    Result.FValues[1] = -c1 * s3 + s1 * s2 * c3;
    Result.FValues[2] = s1 * s3 + c1 * s2 * c3;
    Result.FValues[4] = c2 * s3;
    Result.FValues[5] = c1 * c3 + s1 * s2 * s3;
    Result.FValues[6] = -s1 * c3 + c1 * s2 * s3;
    Result.FValues[8] = -s2;
    Result.FValues[9] = s1 * c2;
    Result.FValues[10] = c1 * c2;
    Result.FValues[15] = 1.f;
    return Result;
}

vec3 ExtractEulerAnglesXYZ(const mat4& m)
{
    float T1 = Atan2(m.FValues[9], m.FValues[10]);
    float C2 = sqrtf(m.FValues[0] * m.FValues[0] + m.FValues[4] * m.FValues[4]);
    float T2 = Atan2(-m.FValues[8], C2);
    float S1 = sinf(T1);
    float C1 = cosf(T1);
    float T3 = Atan2(S1 * m.FValues[2] - C1 * m.FValues[1], C1 * m.FValues[5] - S1 * m.FValues[6]);
    return vec3(-T1, -T2, -T3);
}

mat4 LookAt(const vec3& parEye, const vec3& parCenter, const vec3& parUp)
{
    const vec3 f = Normalize(parCenter - parEye);
    const vec3 s = Normalize(Cross(parUp, f));
    const vec3 u = Cross(f, s);

    const vec4 t = vec4(-Dot(s, parEye), -Dot(u, parEye), -Dot(f, parEye), 1.f);

    mat4 res = mat4::Identity();
    res.SetRow(0, vec4(s, 0.f));
    res.SetRow(1, vec4(u, 0.f));
    res.SetRow(2, vec4(f, 0.f));
    res.SetColumn(3, t);
    return res;
}

} // namespace ECSEngine
