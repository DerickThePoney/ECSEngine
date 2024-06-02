#include "stdafx.h"

#include "Math/Matrix.h"
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

        b *= 2.f;
        CHECK(b.x == 4.f);
        CHECK(b.y == 4.f);
        CHECK(b.z == 4.f);
        CHECK(b.w == 4.f);
    }

    TEST_CASE("Quaternion comparison")
    {
        vec4 one(1.f);
        quat q1(one);
        quat q2(one * 2.f);
        quat q3(one);

        SUBCASE("Equality")
        {
            const bool equal = q1 == q3;
            const bool diff = q2 == q1;
            CHECK(equal);
            CHECK(!diff);
        }
        SUBCASE("Difference")
        {
            const bool equal = q1 != q3;
            const bool diff = q2 != q1;
            CHECK(!equal);
            CHECK(diff);
        }

        SUBCASE("Matrix conversion")
        {
            quat q1(0.f, 0.f, 0.f, 1.f);
            mat4 m = (mat4)q1;
            CHECK(m.FValues[0] == 1.f);
            CHECK(m.FValues[1] == 0.f);
            CHECK(m.FValues[2] == 0.f);
            CHECK(m.FValues[3] == 0.f);

            CHECK(m.FValues[4] == 0.f);
            CHECK(m.FValues[5] == 1.f);
            CHECK(m.FValues[6] == 0.f);
            CHECK(m.FValues[7] == 0.f);

            CHECK(m.FValues[8] == 0.f);
            CHECK(m.FValues[9] == 0.f);
            CHECK(m.FValues[10] == 1.f);
            CHECK(m.FValues[11] == 0.f);

            CHECK(m.FValues[12] == 0.f);
            CHECK(m.FValues[13] == 0.f);
            CHECK(m.FValues[14] == 0.f);
            CHECK(m.FValues[15] == 1.f);
        }
    }
}