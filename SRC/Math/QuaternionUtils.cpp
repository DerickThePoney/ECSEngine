#include "QuaternionUtils.h"
#include "Quaternion.h"
#include "math.h"

namespace ECSEngine
{

float Dot(const quat& parA, const quat& parB)
{
    return parA.x * parB.x + parA.y * parB.y + parA.z * parB.z + parA.w * parB.w;
}

quat Normalize(const quat& parQ)
{
    const float lengthSq = Dot(parQ, parQ);
    return parQ * (1.f / sqrtf(lengthSq));
}

}

