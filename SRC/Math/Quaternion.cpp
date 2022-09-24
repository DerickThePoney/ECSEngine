#include "Quaternion.h"
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

}

