#include "Matrix.h"
#include "Vector.h"
#include "Common/Assertions.h"
#include "MatrixUtils.h"
#include "VectorUtils.h"

namespace ECSEngine
{
/****************************
 * Matrix2x2f
 ****************************/
Matrix2x2f::Matrix2x2f(float parValue)
{
    FValues[0] = parValue;
    FValues[1] = parValue;
    FValues[2] = parValue;
    FValues[3] = parValue;
}

Matrix2x2f::Matrix2x2f(const vec2& A, const vec2& B)
{
    FValues[0] = A.x;
    FValues[1] = B.x;
    FValues[2] = A.y;
    FValues[3] = B.y;
}

const vec2 Matrix2x2f::Row(u32 idx) const
{
    AssertRelease(idx < Size);
    return vec2(FValues[idx * Size], FValues[idx * Size + 1]);
}

const vec2 Matrix2x2f::Column(u32 idx) const
{
    AssertRelease(idx < Size);
    return vec2(FValues[idx], FValues[Size + idx]);
}

void Matrix2x2f::SetRow(u32 idx, const vec2& row)
{
    AssertRelease(idx < Size);
    FValues[idx * Size] = row.x;
    FValues[idx * Size + 1] = row.y;
}

void Matrix2x2f::SetColumn(u32 idx, const vec2& column)
{
    AssertRelease(idx < Size);
    FValues[idx] = column.x;
    FValues[Size + idx] = column.y;
}

Matrix2x2f Matrix2x2f::Identity()
{
    Matrix2x2f result;
    result.FValues[0] = 1.f;
    result.FValues[3] = 1.f;
    return result;
}

Matrix2x2f Matrix2x2f::operator*(const Matrix2x2f& parA)
{
    return Mul(*this, parA);
}

vec2 Matrix2x2f::operator*(const vec2& parA)
{
    return vec2(Dot(Row(0), parA), Dot(Row(1), parA));
}

Matrix2x2f Matrix2x2f::operator+(const Matrix2x2f& parA)
{
    return Add(*this, parA);
}

Matrix2x2f Matrix2x2f::operator-(const Matrix2x2f& parA)
{
    return Sub(*this, parA);
}
/****************************
 * Matrix3x3f
 ****************************/

Matrix3x3f::Matrix3x3f(float parValue)
{
     FValues[0] = parValue;
     FValues[1] = parValue;
     FValues[2] = parValue;
     FValues[3] = parValue;
     FValues[4] = parValue;
     FValues[5] = parValue;
     FValues[6] = parValue;
     FValues[7] = parValue;
     FValues[8] = parValue;
 }

 Matrix3x3f::Matrix3x3f(const vec3& A, const vec3& B, const vec3& C)
 {
     FValues[0] = A.x;
     FValues[1] = B.x;
     FValues[2] = C.x;
     FValues[3] = A.y;
     FValues[4] = B.y;
     FValues[5] = C.y;
     FValues[6] = A.z;
     FValues[7] = B.z;
     FValues[8] = C.z;
 }

const vec3 Matrix3x3f::Row(u32 idx) const
{
    AssertRelease(idx < Size);
    return vec3(FValues[idx * Size], FValues[idx * Size + 1], FValues[idx * Size + 2]);
}

const vec3 Matrix3x3f::Column(u32 idx) const
{
    AssertRelease(idx < Size);
    return vec3(FValues[idx], FValues[Size + idx], FValues[2 * Size + idx]);
}

void Matrix3x3f::SetRow(u32 idx, const vec3& row)
{
    AssertRelease(idx < Size);
    FValues[idx * Size] = row.x;
    FValues[idx * Size + 1] = row.y;
    FValues[idx * Size + 2] = row.z;
}

void Matrix3x3f::SetColumn(u32 idx, const vec3& column)
{
    AssertRelease(idx < Size);
    FValues[idx] = column.x;
    FValues[Size + idx] = column.y;
    FValues[2 * Size + idx] = column.z;
}

Matrix3x3f Matrix3x3f::operator*(const Matrix3x3f& parA)
{
    return Mul(*this, parA);
}

vec3 Matrix3x3f::operator*(const vec3& parA)
{
    return vec3(Dot(Row(0), parA), Dot(Row(1), parA), Dot(Row(2), parA));
}

Matrix3x3f Matrix3x3f::operator+(const Matrix3x3f& parA)
{
    return Add(*this, parA);
}

Matrix3x3f Matrix3x3f::operator-(const Matrix3x3f& parA)
{
    return Sub(*this, parA);
}

Matrix3x3f Matrix3x3f::Identity()
{
    Matrix3x3f result;
    result.FValues[0] = 1.f;
    result.FValues[4] = 1.f;
    result.FValues[8] = 1.f;
    return result;
}

/****************************
 * Matrix4x4f
 ****************************/

//TODO - Revisit row - column order and the order or operations here... I think what GD mean by Av is vA

Matrix4x4f::Matrix4x4f(float parValue)
{
    for (u32 i = 0; i < Matrix4x4f::Size * Matrix4x4f::Size; ++i)
        FValues[i] = parValue;
}

Matrix4x4f::Matrix4x4f(const vec4& A, const vec4& B, const vec4& C, const vec4& D)
{
    FValues[0] = A.x;
    FValues[1] = B.x;
    FValues[2] = C.x;
    FValues[3] = D.x;
    FValues[4] = A.y;
    FValues[5] = B.y;
    FValues[6] = C.y;
    FValues[7] = D.y;
    FValues[8] = A.z;
    FValues[9] = B.z;
    FValues[10] = C.z;
    FValues[11] = D.z;
    FValues[12] = A.w;
    FValues[13] = B.w;
    FValues[14] = C.w;
    FValues[15] = D.w;
}

const vec4 Matrix4x4f::Row(u32 idx) const
{
    AssertRelease(idx < Size);
    return vec4(FValues[idx * Size], FValues[idx * Size + 1], FValues[idx * Size + 2], FValues[idx * Size + 3]);
}

const vec4 Matrix4x4f::Column(u32 idx) const
{
    AssertRelease(idx < Size);
    return vec4(FValues[idx], FValues[Size + idx], FValues[2 * Size + idx], FValues[3 * Size + idx]);
}

void Matrix4x4f::SetRow(u32 idx, const vec4& row)
{
    AssertRelease(idx < Size);
    FValues[idx * Size] = row.x;
    FValues[idx * Size + 1] = row.y;
    FValues[idx * Size + 2] = row.z;
    FValues[idx * Size + 3] = row.w;
}

void Matrix4x4f::SetColumn(u32 idx, const vec4& column)
{
    AssertRelease(idx < Size);
    FValues[idx] = column.x;
    FValues[Size + idx] = column.y;
    FValues[2 * Size + idx] = column.z;
    FValues[3 * Size + idx] = column.w;
}

Matrix4x4f Matrix4x4f::operator*(const Matrix4x4f& parA)
{
    return Mul(*this, parA);
}

vec4 Matrix4x4f::operator*(const vec4& parA)
{
    return vec4(Dot(Row(0), parA), Dot(Row(1), parA), Dot(Row(2), parA), Dot(Row(3), parA));
}

Matrix4x4f Matrix4x4f::operator+(const Matrix4x4f& parA)
{
    return Add(*this, parA);
}

Matrix4x4f Matrix4x4f::operator-(const Matrix4x4f& parA)
{
    return Sub(*this, parA);
}

Matrix4x4f Matrix4x4f::Identity()
{
    Matrix4x4f result;
    result.FValues[0] = 1.f;
    result.FValues[5] = 1.f;
    result.FValues[10] = 1.f;
    result.FValues[15] = 1.f;
    return result;
}

}

