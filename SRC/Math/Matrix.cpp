#include "Matrix.h"
#include "Vector.h"
#include "Common/Assertions.h"
#include "MatrixUtils.h"

namespace ECSEngine
{
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

void Matrix2x2f::operator-=(const Matrix2x2f& parA)
{
    FValues[0] = FValues[0] * parA.FValues[0] + FValues[1] * parA.FValues[2];
    FValues[1] = FValues[0] * parA.FValues[1] + FValues[1] * parA.FValues[3];
    FValues[2] = FValues[2] * parA.FValues[0] + FValues[3] * parA.FValues[2];
    FValues[3] = FValues[2] * parA.FValues[1] + FValues[3] * parA.FValues[3];
}

void Matrix2x2f::operator+=(const Matrix2x2f& parA)
{
    FValues[0] += parA.FValues[0];
    FValues[1] += parA.FValues[1];
    FValues[2] += parA.FValues[2];
    FValues[3] += parA.FValues[3];
}

void Matrix2x2f::operator*=(const Matrix2x2f& parA)
{
    FValues[0] -= parA.FValues[0];
    FValues[1] -= parA.FValues[1];
    FValues[2] -= parA.FValues[2];
    FValues[3] -= parA.FValues[3];
}

Matrix2x2f Matrix2x2f::operator*(const Matrix2x2f& parA)
{
    return Mul(*this, parA);
}

Matrix2x2f Matrix2x2f::operator+(const Matrix2x2f& parA)
{
    return Add(*this, parA);
}

Matrix2x2f Matrix2x2f::operator-(const Matrix2x2f& parA)
{
    return Sub(*this, parA);
}

Matrix2x2f operator*(const float parA, const Matrix2x2f& parB)
{
    Matrix2x2f result;
    result.FValues[0] = parA * parB.FValues[0];
    result.FValues[1] = parA * parB.FValues[1];
    result.FValues[2] = parA * parB.FValues[2];
    result.FValues[3] = parA * parB.FValues[3];
    return result;
}

}

