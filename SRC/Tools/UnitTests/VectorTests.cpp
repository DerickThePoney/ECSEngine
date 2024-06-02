#include "stdafx.h"

#include "Math/Vector.h"
#include "Math/VectorUtils.h"
#include "doctest.h"

template<typename VecType>
void Vec2UnitTests()
{
    {
        VecType a;
        CHECK(a.x == 0.f);
        CHECK(a.y == 0.f);
    }

    {
        VecType b(1);
        CHECK(b.x == 1);
        CHECK(b.y == 1);
    }

    {
        VecType c(1, 2);
        CHECK(c.x == 1);
        CHECK(c.y == 2);
    }

    {
        VecType test1(1.f, 2.f);
        VecType test2(2.f, 1.f);

        VecType somme = test1 + test2;
        CHECK(somme.x == 3);
        CHECK(somme.y == 3);
    }

    {
        VecType test1(2.f, 2.f);
        VecType test2(1.f, 1.f);

        VecType soustraction = test1 - test2;
        CHECK(soustraction.x == 1.f);
        CHECK(soustraction.y == 1.f);
    }

    {
        VecType test1(2.f, 2.f);
        VecType test2(2.f, 2.f);

        VecType multiplication = test1 * test2;
        CHECK(multiplication.x == 4.f);
        CHECK(multiplication.y == 4.f);
    }

    {
        VecType test1(2.f, 2.f);
        VecType test2(2.f, 2.f);

        VecType div = test1 / test2;
        CHECK(div.x == 1.f);
        CHECK(div.y == 1.f);
    }

    {
        VecType test1(1.f, 2.f);
        VecType test2(2.f, 1.f);

        test1 += test2;
        CHECK(test1.x == 3.f);
        CHECK(test1.y == 3.f);
    }

    {
        VecType test1(2.f, 2.f);
        VecType test2(1.f, 1.f);

        test1 -= test2;
        CHECK(test1.x == 1.f);
        CHECK(test1.y == 1.f);
    }

    {
        VecType test1(2.f, 2.f);
        VecType test2(2.f, 2.f);

        test1 *= test2;
        CHECK(test1.x == 4.f);
        CHECK(test1.y == 4.f);
    }

    {
        VecType test1(2.f, 2.f);
        VecType test2(2.f, 2.f);

        test1 /= test2;
        CHECK(test1.x == 1.f);
        CHECK(test1.y == 1.f);
    }

    {
        VecType test1(2.f, 2.f);
        VecType test2 = test1;

        CHECK(test1.x == test2.x);
        CHECK(test1.y == test2.y);
    }

    {
        VecType test1(2.f, 2.f);
        VecType test2(2.f, 2.f);
        VecType test3(2.f, 1.f);

        const bool equal = test1 == test2;
        const bool diff = test1 == test3;

        CHECK(equal);
        CHECK(!diff);
    }

    {
        VecType test1(2.f, 2.f);
        VecType test2(2.f, 2.f);
        VecType test3(2.f, 1.f);

        const bool equal = test1 != test2;
        const bool diff = test1 != test3;

        CHECK(!equal);
        CHECK(diff);
    }

    {
        VecType test1(3.f, 4.f);

        const float lengthSq = LengthSq(test1);
        const float length = Length(test1);

        CHECK(lengthSq == 25.f);
        CHECK(length == 5.f);
    }
}

template<typename VecType>
void Vec3UnitTests()
{

    {
        VecType test1(1.f, 2.f, 3.0f);
        VecType test2(2.f, 1.f, 3.0f);

        VecType somme = test1 + test2;
        CHECK(somme.x == 3.f);
        CHECK(somme.y == 3.f);
        CHECK(somme.z == 6.f);
    }

    {
        VecType test1(2.f, 2.f, 3.0f);
        VecType test2(1.f, 1.f, 3.0f);

        VecType soustraction = test1 - test2;
        CHECK(soustraction.x == 1.f);
        CHECK(soustraction.y == 1.f);
        CHECK(soustraction.z == 0.f);
    }

    {
        VecType test1(2.f, 2.f, 3.0f);
        VecType test2(2.f, 2.f, 3.0f);

        VecType multiplication = test1 * test2;
        CHECK(multiplication.x == 4.f);
        CHECK(multiplication.y == 4.f);
        CHECK(multiplication.z == 9.f);
    }

    {
        VecType test1(2.f, 2.f, 3.0f);
        VecType test2(2.f, 2.f, 3.0f);

        VecType div = test1 / test2;
        CHECK(div.x == 1.f);
        CHECK(div.y == 1.f);
        CHECK(div.z == 1.f);
    }

    {
        VecType test1(1.f, 2.f, 3.0f);
        VecType test2(2.f, 1.f, 3.0f);

        test1 += test2;
        CHECK(test1.x == 3.f);
        CHECK(test1.y == 3.f);
        CHECK(test1.z == 6.f);
    }

    {
        VecType test1(2.f, 2.f, 3.0f);
        VecType test2(1.f, 1.f, 3.0f);

        test1 -= test2;
        CHECK(test1.x == 1.f);
        CHECK(test1.y == 1.f);
        CHECK(test1.z == 0.f);
    }

    {
        VecType test1(2.f, 2.f, 3.0f);
        VecType test2(2.f, 2.f, 3.0f);

        test1 *= test2;
        CHECK(test1.x == 4.f);
        CHECK(test1.y == 4.f);
        CHECK(test1.z == 9.f);
    }

    {
        VecType test1(2.f, 2.f, 3.0f);
        VecType test2(2.f, 2.f, 3.0f);

        test1 /= test2;
        CHECK(test1.x == 1.f);
        CHECK(test1.y == 1.f);
        CHECK(test1.z == 1.f);
    }

    {
        VecType test1(2.f, 2.f, 3.0f);
        VecType test2 = test1;

        CHECK(test1.x == 2.f);
        CHECK(test1.y == 2.f);
        CHECK(test1.z == 3.f);
    }

    {
        VecType test1(2.f, 2.f, 3.0f);
        VecType test2(2.f, 2.f, 3.0f);
        VecType test3(2.f, 1.f, 3.0f);

        const bool equal = test1 == test2;
        const bool diff = test1 == test3;

        CHECK(equal);
        CHECK(!diff);
    }

    {
        VecType test1(2.f, 2.f, 3.0f);
        VecType test2(2.f, 2.f, 3.0f);
        VecType test3(2.f, 1.f, 3.0f);

        const bool equal = test1 != test2;
        const bool diff = test1 != test3;

        CHECK(!equal);
        CHECK(diff);
    }

    {
        VecType test2(2.f, 9.f, 3.0f);

        auto test1 = test2.xx();
        CHECK(test1.x == test2.x);
        CHECK(test1.y == test2.x);

        test1 = test2.yy();
        CHECK(test1.x == test2.y);
        CHECK(test1.y == test2.y);

        test1 = test2.yx();
        CHECK(test1.x == test2.y);
        CHECK(test1.y == test2.x);

        test1 = test2.x0();
        CHECK(test1.x == test2.x);
        CHECK(test1.y == 0.f);

        test1 = test2.x1();
        CHECK(test1.x == test2.x);
        CHECK(test1.y == 1.f);

        test1 = test2.y0();
        CHECK(test1.x == test2.y);
        CHECK(test1.y == 0.f);

        test1 = test2.y1();
        CHECK(test1.x == test2.y);
        CHECK(test1.y == 1.f);
    }

    {
        VecType test1(2.f, 2.f, 2.f);

        const float lengthSq = LengthSq(test1);

        CHECK(lengthSq == 12.f);
    }
}

template<typename VecType>
void Vec4UnitTests()
{

    {
        VecType test1(1.f, 2.f, 3.0f, 5.0f);
        VecType test2(2.f, 1.f, 3.0f, 5.0f);

        VecType somme = test1 + test2;
        CHECK(somme.x == 3.f);
        CHECK(somme.y == 3.f);
        CHECK(somme.z == 6.f);
        CHECK(somme.w == 10.f);
    }

    {
        VecType test1(2.f, 2.f, 3.0f, 5.0f);
        VecType test2(1.f, 1.f, 3.0f, 5.0f);

        VecType soustraction = test1 - test2;
        CHECK(soustraction.x == 1.f);
        CHECK(soustraction.y == 1.f);
        CHECK(soustraction.z == 0.f);
        CHECK(soustraction.w == 0.f);
    }

    {
        VecType test1(2.f, 2.f, 3.0f, 5.0f);
        VecType test2(2.f, 2.f, 3.0f, 5.0f);

        VecType multiplication = test1 * test2;
        CHECK(multiplication.x == 4.f);
        CHECK(multiplication.y == 4.f);
        CHECK(multiplication.z == 9.f);
        CHECK(multiplication.w == 25.f);
    }

    {
        VecType test1(2.f, 2.f, 3.0f, 5.0f);
        VecType test2(2.f, 2.f, 3.0f, 5.0f);

        VecType div = test1 / test2;
        CHECK(div.x == 1.f);
        CHECK(div.y == 1.f);
        CHECK(div.z == 1.f);
        CHECK(div.w == 1.f);
    }

    {
        VecType test1(1.f, 2.f, 3.0f, 5.0f);
        VecType test2(2.f, 1.f, 3.0f, 5.0f);

        test1 += test2;
        CHECK(test1.x == 3.f);
        CHECK(test1.y == 3.f);
        CHECK(test1.z == 6.f);
        CHECK(test1.w == 10.f);
    }

    {
        VecType test1(2.f, 2.f, 3.0f, 5.0f);
        VecType test2(1.f, 1.f, 3.0f, 5.0f);

        test1 -= test2;
        CHECK(test1.x == 1.f);
        CHECK(test1.y == 1.f);
        CHECK(test1.z == 0.f);
        CHECK(test1.w == 0.f);
    }

    {
        VecType test1(2.f, 2.f, 3.0f, 5.0f);
        VecType test2(2.f, 2.f, 3.0f, 5.0f);

        test1 *= test2;
        CHECK(test1.x == 4.f);
        CHECK(test1.y == 4.f);
        CHECK(test1.z == 9.f);
        CHECK(test1.w == 25.f);
    }

    {
        VecType test1(2.f, 2.f, 3.0f, 5.0f);
        VecType test2(2.f, 2.f, 3.0f, 5.0f);

        test1 /= test2;
        CHECK(test1.x == 1.f);
        CHECK(test1.y == 1.f);
        CHECK(test1.z == 1.f);
        CHECK(test1.w == 1.f);
    }

    {
        VecType test1(2.f, 2.f, 3.0f, 5.0f);
        VecType test2 = test1;

        CHECK(test1.x == 2.f);
        CHECK(test1.y == 2.f);
        CHECK(test1.z == 3.f);
        CHECK(test1.w == 5.f);
    }

    {
        VecType test1(2.f, 2.f, 3.0f, 5.0f);
        VecType test2(2.f, 2.f, 3.0f, 5.0f);
        VecType test3(2.f, 1.f, 3.0f, 5.0f);

        const bool equal = test1 == test2;
        const bool diff = test1 == test3;

        CHECK(equal);
        CHECK(!diff);
    }

    {
        VecType test1(2.f, 2.f, 3.0f, 5.0f);
        VecType test2(2.f, 2.f, 3.0f, 5.0f);
        VecType test3(2.f, 1.f, 3.0f, 5.0f);

        const bool equal = test1 != test2;
        const bool diff = test1 != test3;

        CHECK(!equal);
        CHECK(diff);
    }

    {
        VecType test2(2.f, 9.f, 3.0f, 5.0f);

        auto test1 = test2.xx();
        CHECK(test1.x == test2.x);
        CHECK(test1.y == test2.x);

        test1 = test2.yy();
        CHECK(test1.x == test2.y);
        CHECK(test1.y == test2.y);

        test1 = test2.yx();
        CHECK(test1.x == test2.y);
        CHECK(test1.y == test2.x);

        test1 = test2.x0();
        CHECK(test1.x == test2.x);
        CHECK(test1.y == 0.f);

        test1 = test2.x1();
        CHECK(test1.x == test2.x);
        CHECK(test1.y == 1.f);

        test1 = test2.y0();
        CHECK(test1.x == test2.y);
        CHECK(test1.y == 0.f);

        test1 = test2.y1();
        CHECK(test1.x == test2.y);
        CHECK(test1.y == 1.f);
    }

    {
        VecType test1(2.f, 2.f, 2.f, 2.f);

        const float lengthSq = LengthSq(test1);

        CHECK(lengthSq == 16.f);
    }

    {
        VecType test1(2.f, 2.f, 2.f, 2.f);
        VecType HV2P = VecType::MakeHomogeneousPositionVec4(test1.xy());
        VecType HV2D = VecType::MakeHomogeneousDirectionVec4(test1.xy());
        VecType HV3P = VecType::MakeHomogeneousPositionVec4(test1.xyz());
        VecType HV3D = VecType::MakeHomogeneousDirectionVec4(test1.xyz());
        CHECK(HV2P.xy() == test1.xy());
        CHECK(HV2P.z == 0);
        CHECK(HV2P.w == 1);

        CHECK(HV2D.xy() == test1.xy());
        CHECK(HV2D.z == 0);
        CHECK(HV2D.w == 0);

        CHECK(HV3P.xyz() == test1.xyz());
        CHECK(HV3P.w == 1);

        CHECK(HV3D.xyz() == test1.xyz());
        CHECK(HV3D.w == 0);
    }
}

TEST_SUITE("Vector tests")
{
    TEST_CASE("Vec2 tests")
    {
        SUBCASE("Vec2 floats")
        {
            Vec2UnitTests<ECSEngine::vec2>();
        }

        SUBCASE("Vec2 floats special")
        {
            ECSEngine::vec2 a = ECSEngine::vec2(2.f, 2.f);
            ECSEngine::vec2 b = ECSEngine::vec2(2.f, 2.f);

            const float dot_a_b = ECSEngine::Dot(a, b);
            CHECK(dot_a_b == 8.f);

            ECSEngine::vec2 an = ECSEngine::Normalize(a);
            const float length = ECSEngine::LengthSq(an);
            auto abs = [](const float a) { return a >= 0 ? a : -a; };
            CHECK(abs(length - 1.f) < 1.e-3);
        }

        SUBCASE("Vec2 int")
        {
            Vec2UnitTests<ECSEngine::ivec2>();
        }

        SUBCASE("Vec2 uint")
        {
            Vec2UnitTests<ECSEngine::uvec2>();
        }
    }

    TEST_CASE("Vec3 tests")
    {
        SUBCASE("Vec3 floats")
        {
            Vec3UnitTests<ECSEngine::vec3>();
        }

        SUBCASE("Vec3 floats dot")
        {
            ECSEngine::vec3 a = ECSEngine::vec3(2.f, 2.f, 2.f);
            ECSEngine::vec3 b = ECSEngine::vec3(2.f, 2.f, 2.f);

            const float dot_a_b = ECSEngine::Dot(a, b);
            CHECK(dot_a_b == 12.f);

            ECSEngine::vec3 an = ECSEngine::Normalize(a);
            const float length = ECSEngine::LengthSq(an);
            auto abs = [](const float a) { return a >= 0 ? a : -a; };
            CHECK(abs(length - 1.f) < 1.e-3);

            ECSEngine::vec3 c = ECSEngine::vec3(1.f, 0.f, 0.f);
            ECSEngine::vec3 d = ECSEngine::vec3(0.f, 1.f, 0.f);
            ECSEngine::vec3 e = ECSEngine::Cross(c, d);
            ECSEngine::vec3 f = ECSEngine::Cross(d, c);
            CHECK(e.x == 0.f);
            CHECK(e.y == 0.f);
            CHECK(e.z == 1.f);
            CHECK(f.x == 0.f);
            CHECK(f.y == 0.f);
            CHECK(f.z == -1.f);
        }

        SUBCASE("Vec3 int")
        {
            Vec3UnitTests<ECSEngine::ivec3>();
        }

        SUBCASE("Vec3 uint")
        {
            Vec3UnitTests<ECSEngine::uvec3>();
        }
    }

    TEST_CASE("Vec4 tests")
    {
        SUBCASE("Vec4 floats")
        {
            Vec4UnitTests<ECSEngine::vec4>();
        }

        SUBCASE("Vec4 floats dot")
        {
            ECSEngine::vec4 a = ECSEngine::vec4(2.f, 2.f, 2.f, 2.f);
            ECSEngine::vec4 b = ECSEngine::vec4(2.f, 2.f, 2.f, 2.f);

            const float dot_a_b = ECSEngine::Dot(a, b);
            CHECK(dot_a_b == 16.f);

            ECSEngine::vec4 an = ECSEngine::Normalize(a);
            const float length = ECSEngine::LengthSq(an);
            auto abs = [](const float a) { return a >= 0 ? a : -a; };
            CHECK(abs(length - 1.f) < 1.e-3);
        }

        SUBCASE("Vec4 int")
        {
            Vec4UnitTests<ECSEngine::ivec4>();
        }

        SUBCASE("Vec4 uint")
        {
            Vec4UnitTests<ECSEngine::uvec4>();
        }
    }
}