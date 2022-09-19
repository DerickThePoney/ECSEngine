#include "Matrix.h"
#include "Vector.h"
#include "Common/Assertions.h"

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

}

