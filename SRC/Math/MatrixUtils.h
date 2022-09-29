#pragma once
#include "MatrixTypes.h"
#include "VectorTypes.h"

namespace ECSEngine
{
mat2 Mul(const mat2& parA, const mat2& parB);
mat2 Add(const mat2& parA, const mat2& parB);
mat2 Sub(const mat2& parA, const mat2& parB);

float Determinant(const mat2& parA);
mat2 Invert(const mat2& parA);
mat2 Transpose(const mat2& parA);

mat3 Mul(const mat3& parA, const mat3& parB);
mat3 Add(const mat3& parA, const mat3& parB);
mat3 Sub(const mat3& parA, const mat3& parB);

float Determinant(const mat3& parA);
mat3 Invert(const mat3& parA);
mat3 Transpose(const mat3& parA);

mat4 Mul(const mat4& parA, const mat4& parB);
mat4 Add(const mat4& parA, const mat4& parB);
mat4 Sub(const mat4& parA, const mat4& parB);

float Determinant(const mat4& parA);
mat4 Invert(const mat4& parA);
mat4 Transpose(const mat4& parA);

mat4 Translation(const vec3& parT);
mat4 Perspective(float fovy, float aspect, float zNear, float zFar);
mat4 EulerAnglesXYZ(const float X, const float Y, const float Z);
vec3 ExtractEulerAnglesXYZ(const mat4& m);
}