#include "Quaternion.h"

#include "Matrix.h"
#include "math.h"

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

quat Quaternionf::operator*(const quat& parA)
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

} // namespace ECSEngine
