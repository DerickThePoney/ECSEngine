#include "Quaternion.h"

#include "Matrix.h"
#include "math.h"
#include "Common/Assertions.h"
#include "MathFunctions.h"

namespace ECSEngine
{

Quaternionf::Quaternionf(const vec3& parAxis, const float parAngle)
{
    const float half_angle = 0.5f * parAngle;
    w = cosf(half_angle);
    const float sin_half_angle = sinf(half_angle);
    x = parAxis.x * sin_half_angle;
    y = parAxis.y * sin_half_angle;
    z = parAxis.z * sin_half_angle;
}

quat Quaternionf::operator*(const quat& parA) const
{
    const float nw = w * parA.w - x * parA.x - y * parA.y - z * parA.z;
    const float nx = z * parA.y - y * parA.z + x * parA.w + w * parA.x;
    const float ny = x * parA.z - z * parA.x + y * parA.w + w * parA.y;
    const float nz = y * parA.x - x * parA.y + z * parA.w + w * parA.z;
    return quat(nx, ny, nz, nw);
}

void Quaternionf::operator*=(const quat& parA)
{
    w = w * parA.w - x * parA.x - y * parA.y - z * parA.z;
    x = z * parA.y - y * parA.z + x * parA.w + w * parA.x;
    y = x * parA.z - z * parA.x + y * parA.w + w * parA.y;
    z = y * parA.x - x * parA.y + z * parA.w + w * parA.z;
}

Quaternionf::operator mat4() const
{
    const mat4 a = mat4(vec4(w, z, -y, -x), vec4(-z, w, x, -y), vec4(y, -x, w, -z), vec4(x, y, z, w));
    const mat4 b = mat4(vec4(w, z, -y, x), vec4(-z, w, x, y), vec4(y, -x, w, z), vec4(-x, -y, -z, w));
    return a * b;
}

Quaternionf Quaternionf::FromMat4(const mat4& parMatrix)
{
    //Source glm
    float fourXSquaredMinus1 = parMatrix.FValues[0] - parMatrix.FValues[5] - parMatrix.FValues[10];
    float fourYSquaredMinus1 = parMatrix.FValues[5] - parMatrix.FValues[0] - parMatrix.FValues[10];
    float fourZSquaredMinus1 = parMatrix.FValues[10] - parMatrix.FValues[0] - parMatrix.FValues[5];
    float fourWSquaredMinus1 = parMatrix.FValues[0] + parMatrix.FValues[5] + parMatrix.FValues[10];

    int biggestIndex = 0;
    float fourBiggestSquaredMinus1 = fourWSquaredMinus1;
    if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
    {
        fourBiggestSquaredMinus1 = fourXSquaredMinus1;
        biggestIndex = 1;
    }
    if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
    {
        fourBiggestSquaredMinus1 = fourYSquaredMinus1;
        biggestIndex = 2;
    }
    if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
    {
        fourBiggestSquaredMinus1 = fourZSquaredMinus1;
        biggestIndex = 3;
    }

    float biggestVal = sqrtf(fourBiggestSquaredMinus1 + 1.f) * 0.5f;
    float mult = 0.25f / biggestVal;

    switch (biggestIndex)
    {
    case 0:
        return quat((parMatrix.FValues[9] - parMatrix.FValues[6]) * mult, (parMatrix.FValues[2] - parMatrix.FValues[8]) * mult, (parMatrix.FValues[4] - parMatrix.FValues[1]) * mult, biggestVal);
    case 1:
        return quat(biggestVal, (parMatrix.FValues[4] + parMatrix.FValues[1]) * mult, (parMatrix.FValues[2] + parMatrix.FValues[8]) * mult, (parMatrix.FValues[9] - parMatrix.FValues[6]) * mult);
    case 2:
        return quat((parMatrix.FValues[4] + parMatrix.FValues[1]) * mult, biggestVal, (parMatrix.FValues[9] + parMatrix.FValues[6]) * mult, (parMatrix.FValues[2] - parMatrix.FValues[8]) * mult);
    case 3:
        return quat((parMatrix.FValues[2] + parMatrix.FValues[8]) * mult, (parMatrix.FValues[9] + parMatrix.FValues[6]) * mult, biggestVal, (parMatrix.FValues[4] - parMatrix.FValues[1]) * mult);
    default: 
        AssertNotReached();
        return quat(0, 0, 0, 1);
    }
}

} // namespace ECSEngine
