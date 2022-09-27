#include "stdafx.h"

#include "Math/Quaternion.h"
#include "doctest.h"

using namespace ECSEngine;

TEST_SUITE("Quaternion tests")
{
    TEST_CASE("Quaternion Construction")
    {
        vec4 one(1.f);
        quat a(one);
        CHECK(a.x == 1.f);
        CHECK(a.y == 1.f);
        CHECK(a.z == 1.f);
        CHECK(a.w == 1.f);

        quat b(1.f, 1.f, 1.f, 1.f);
        CHECK(a == b);
    }

    TEST_CASE("Quaternion scalar multiplications")
    {
        vec4 one(1.f);
        quat a(one);
        quat b = a * 2.f;
        CHECK(b.x == 2.f);
        CHECK(b.y == 2.f);
        CHECK(b.z == 2.f);
        CHECK(b.w == 2.f);
    }
}