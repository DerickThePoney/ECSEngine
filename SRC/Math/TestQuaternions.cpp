#include "TestQuaternions.h"

#include "Quaternion.h"
#include "Vector.h"

#include <iostream>

namespace ECSEngine
{
void TestQuaternions()
{
    vec4 one(1.f);
    quat a(one);
    quat b(1.f, 1.f, 1.f, 1.f);

    std::cout << "Cool\n";
}
}

