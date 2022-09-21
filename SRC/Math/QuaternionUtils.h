#pragma once
#include "QuaternionTypes.h"

namespace ECSEngine
{
float Dot(const quat& parA, const quat& parB);
quat Normalize(const quat& parQ);
}
