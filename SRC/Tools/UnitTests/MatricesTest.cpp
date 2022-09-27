#include "stdafx.h"

#include "Math/Matrix.h"
#include "Math/MatrixUtils.h"
#include "Math/Vector.h"
#include "doctest.h"

using namespace ECSEngine;

TEST_SUITE("Test matrices")
{
    TEST_CASE("Matrix2x2f")
    {
        SUBCASE("Construction")
        {
            mat2 t0;
            CHECK(t0.FValues[0] == 0.f);
            CHECK(t0.FValues[1] == 0.f);
            CHECK(t0.FValues[2] == 0.f);
            CHECK(t0.FValues[3] == 0.f);

            mat2 t1(2.f);
            CHECK(t1.FValues[0] == 2.f);
            CHECK(t1.FValues[1] == 2.f);
            CHECK(t1.FValues[2] == 2.f);
            CHECK(t1.FValues[3] == 2.f);

            mat2 t2(vec2(0.f, -1.f), vec2(-1.f, 0.f));
            CHECK(t2.FValues[0] == 0.f);
            CHECK(t2.FValues[1] == -1.f);
            CHECK(t2.FValues[2] == -1.f);
            CHECK(t2.FValues[3] == 0.f);

            mat2 t3 = mat2::Identity();
            CHECK(t3.FValues[0] == 1.f);
            CHECK(t3.FValues[1] == 0.f);
            CHECK(t3.FValues[2] == 0.f);
            CHECK(t3.FValues[3] == 1.f);
        }

        SUBCASE("Matrix mult")
        {
            mat2 t2(vec2(0.f, -1.f), vec2(-1.f, 0.f));
            mat2 t3 = mat2::Identity();
            mat2 t4 = t3 * t2;
            CHECK(t4.FValues[0] == 0.f);
            CHECK(t4.FValues[1] == -1.f);
            CHECK(t4.FValues[2] == -1.f);
            CHECK(t4.FValues[3] == 0.f);

            t4 *= t3;
            CHECK(t4.FValues[0] == 0.f);
            CHECK(t4.FValues[1] == -1.f);
            CHECK(t4.FValues[2] == -1.f);
            CHECK(t4.FValues[3] == 0.f);

            t4 = t4 * 2.f;
            CHECK(t4.FValues[0] == 0.f);
            CHECK(t4.FValues[1] == -2.f);
            CHECK(t4.FValues[2] == -2.f);
            CHECK(t4.FValues[3] == 0.f);
        }
    }

    TEST_CASE("Matrix3x3f")
    {
        mat3 t0(vec3(1.f, 0.f, 5.f), vec3(2.f, 1.f, 6.f), vec3(3.f, 4.f, 0.f));
        mat3 t0_i = Invert(t0);
        mat3 t1 = mat3::Identity() * t0_i;

        SUBCASE("Construction")
        {
            CHECK(t0.FValues[0] == 1.f);
            CHECK(t0.FValues[1] == 2.f);
            CHECK(t0.FValues[2] == 3.f);
            CHECK(t0.FValues[3] == 0.f);
            CHECK(t0.FValues[4] == 1.f);
            CHECK(t0.FValues[5] == 4.f);
            CHECK(t0.FValues[6] == 5.f);
            CHECK(t0.FValues[7] == 6.f);
            CHECK(t0.FValues[8] == 0.f);
        }

        SUBCASE("Inversion")
        {
            CHECK(t0_i.FValues[0] == -24.f);
            CHECK(t0_i.FValues[1] == 18.f);
            CHECK(t0_i.FValues[2] == 5.f);
            CHECK(t0_i.FValues[3] == 20.f);
            CHECK(t0_i.FValues[4] == -15.f);
            CHECK(t0_i.FValues[5] == -4.f);
            CHECK(t0_i.FValues[6] == -5.f);
            CHECK(t0_i.FValues[7] == 4.f);
            CHECK(t0_i.FValues[8] == 1.f);
        }

        SUBCASE("Multiplication")
        {
            CHECK(t1.FValues[0] == -24.f);
            CHECK(t1.FValues[1] == 18.f);
            CHECK(t1.FValues[2] == 5.f);
            CHECK(t1.FValues[3] == 20.f);
            CHECK(t1.FValues[4] == -15.f);
            CHECK(t1.FValues[5] == -4.f);
            CHECK(t1.FValues[6] == -5.f);
            CHECK(t1.FValues[7] == 4.f);
            CHECK(t1.FValues[8] == 1.f);
        }
    }
}