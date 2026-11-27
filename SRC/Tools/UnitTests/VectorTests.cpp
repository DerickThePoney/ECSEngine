#include "stdafx.h"

#include "Math/Vector.h"
#include "Math/VectorUtils.h"
#include "doctest.h"

#include <limits>
#include <type_traits>

static_assert(sizeof(ECSEngine::vec2) == sizeof(float) * 2);
static_assert(alignof(ECSEngine::vec2) == alignof(float));
static_assert(sizeof(ECSEngine::vec3) == sizeof(float) * 3);
static_assert(alignof(ECSEngine::vec3) == alignof(float));
static_assert(sizeof(ECSEngine::vec4) == sizeof(float) * 4);
static_assert(alignof(ECSEngine::vec4) == alignof(float));
static_assert(sizeof(ECSEngine::dvec3) == sizeof(double) * 3);
static_assert(alignof(ECSEngine::dvec3) == alignof(double));

static_assert(std::is_same_v<decltype(ECSEngine::Dot(ECSEngine::vec3{}, ECSEngine::vec3{})), float>);
static_assert(std::is_same_v<decltype(ECSEngine::Length(ECSEngine::vec3{})), float>);
static_assert(std::is_same_v<decltype(ECSEngine::LengthSq(ECSEngine::vec3{})), float>);
static_assert(std::is_same_v<decltype(ECSEngine::ooLength(ECSEngine::vec3{})), float>);
static_assert(std::is_same_v<decltype(ECSEngine::Normalize(ECSEngine::vec3{})), ECSEngine::vec3>);

static_assert(std::is_same_v<decltype(ECSEngine::Dot(ECSEngine::dvec3{}, ECSEngine::dvec3{})), double>);
static_assert(std::is_same_v<decltype(ECSEngine::Length(ECSEngine::dvec3{})), double>);
static_assert(std::is_same_v<decltype(ECSEngine::LengthSq(ECSEngine::dvec3{})), double>);
static_assert(std::is_same_v<decltype(ECSEngine::ooLength(ECSEngine::dvec3{})), double>);
static_assert(std::is_same_v<decltype(ECSEngine::Normalize(ECSEngine::dvec3{})), ECSEngine::dvec3>);
static_assert(std::is_same_v<decltype(ECSEngine::Cross(ECSEngine::dvec3{}, ECSEngine::dvec3{})), ECSEngine::dvec3>);

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

        CHECK(test2 == test1);
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

        CHECK(test2 == test1);
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

        SUBCASE("Vec2 scalar on the left")
        {
            const ECSEngine::vec2 value(2.f, 4.f);
            const ECSEngine::vec2 subtracted = 8.f - value;
            CHECK(subtracted.x == 6.f);
            CHECK(subtracted.y == 4.f);

            const ECSEngine::vec2 divided = 8.f / value;
            CHECK(divided.x == 4.f);
            CHECK(divided.y == 2.f);

            CHECK(!ECSEngine::IsNan(value));
            CHECK(ECSEngine::IsNan(ECSEngine::vec2(std::numeric_limits<float>::quiet_NaN(), 1.f)));
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

        SUBCASE("Vec3 floats soustraction division")
        {
            float a = 3.f;
            ECSEngine::vec3 test1(2.f, 2.f, 3.0f);

            ECSEngine::vec3 soustraction = a - test1;
            ECSEngine::vec3 soustraction2 = test1 - a;
            CHECK(soustraction.x == 1.f);
            CHECK(soustraction.y == 1.f);
            CHECK(soustraction.z == 0.f);
            CHECK(soustraction2.x == -1.f);
            CHECK(soustraction2.y == -1.f);
            CHECK(soustraction2.z == 0.f);

            ECSEngine::vec3 div = a / test1;
            ECSEngine::vec3 div2 = test1 / a;
            CHECK(div.x == 3.f / 2.f);
            CHECK(div.y == 3.f / 2.f);
            CHECK(div.z == 1.f);
            CHECK(div2.x == 2.f / 3.f);
            CHECK(div2.y == 2.f / 3.f);
            CHECK(div2.z == 1.f);
        }

        SUBCASE("Vec3 length, normalize and component ops")
        {
            const ECSEngine::vec3 triple(3.f, 4.f, 0.f);
            CHECK(ECSEngine::Length(triple) == 5.f);
            CHECK(ECSEngine::ooLength(triple) * ECSEngine::Length(triple) == doctest::Approx(1.f));

            const ECSEngine::vec3 axis = ECSEngine::Normalize(ECSEngine::vec3(0.f, 5.f, 0.f));
            CHECK(axis == ECSEngine::vec3(0.f, 1.f, 0.f));

            const ECSEngine::vec3 from(0.f, 0.f, 0.f);
            const ECSEngine::vec3 to(10.f, 0.f, -10.f);
            CHECK(ECSEngine::Lerp(from, to, 0.f) == from);
            CHECK(ECSEngine::Lerp(from, to, 1.f) == to);
            CHECK(ECSEngine::Lerp(from, to, 0.5f) == ECSEngine::vec3(5.f, 0.f, -5.f));

            const ECSEngine::vec3 low(1.f, 5.f, -2.f);
            const ECSEngine::vec3 high(3.f, 4.f, 0.f);
            CHECK(ECSEngine::VecMin(low, high) == ECSEngine::vec3(1.f, 4.f, -2.f));
            CHECK(ECSEngine::VecMax(low, high) == ECSEngine::vec3(3.f, 5.f, 0.f));
            CHECK(ECSEngine::Abs(ECSEngine::vec3(-1.f, 2.f, -3.f)) == ECSEngine::vec3(1.f, 2.f, 3.f));
            CHECK(ECSEngine::Invert(ECSEngine::vec3(1.f, -2.f, 3.f)) == ECSEngine::vec3(-1.f, 2.f, -3.f));

            const ECSEngine::vec3 indexed(4.f, 5.f, 6.f);
            CHECK(indexed[0] == 4.f);
            CHECK(indexed[1] == 5.f);
            CHECK(indexed[2] == 6.f);

            ECSEngine::vec3 accumulated(1.f, 2.f, 3.f);
            accumulated += 1.f;
            accumulated *= 2.f;
            CHECK(accumulated == ECSEngine::vec3(4.f, 6.f, 8.f));

            const ECSEngine::vec3 fromVec2(ECSEngine::vec2(4.f, 5.f));
            CHECK(fromVec2 == ECSEngine::vec3(4.f, 5.f, 0.f));

            CHECK(!ECSEngine::IsNan(indexed));
            CHECK(ECSEngine::IsNan(ECSEngine::vec3(1.f, std::numeric_limits<float>::quiet_NaN(), 3.f)));
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

        SUBCASE("Vec4 length and w-preserving invert")
        {
            const ECSEngine::vec4 triple(1.f, 2.f, 2.f, 4.f);
            CHECK(ECSEngine::Length(triple) == 5.f);

            const ECSEngine::vec4 built(ECSEngine::vec3(1.f, 2.f, 3.f));
            CHECK(built == ECSEngine::vec4(1.f, 2.f, 3.f, 0.f));
            CHECK(built[3] == 0.f);

            const ECSEngine::vec4 flipped = ECSEngine::Invert(ECSEngine::vec4(1.f, 2.f, 3.f, 4.f));
            CHECK(flipped == ECSEngine::vec4(-1.f, -2.f, -3.f, 4.f));

            CHECK(!ECSEngine::IsNan(built));
            CHECK(ECSEngine::IsNan(ECSEngine::vec4(1.f, 2.f, 3.f, std::numeric_limits<float>::quiet_NaN())));
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

    TEST_CASE("Dvec3 tests")
    {
        SUBCASE("Same arithmetic as vec3")
        {
            Vec3UnitTests<ECSEngine::dvec3>();
        }

        SUBCASE("Double precision")
        {
            const ECSEngine::dvec3 origin(1.0e10, -2.0e10, 3.0e10);
            const ECSEngine::dvec3 delta(0.25, -0.5, 0.125);
            const ECSEngine::dvec3 sum = origin + delta;
            CHECK(sum.x - origin.x == 0.25);
            CHECK(sum.y - origin.y == -0.5);
            CHECK(sum.z - origin.z == 0.125);
            CHECK(static_cast<float>(origin.x) + 0.25f == static_cast<float>(origin.x));

            const double pastFloat = 16777217.0;
            const ECSEngine::dvec3 wide(pastFloat, 0.0, 0.0);
            CHECK(ECSEngine::Length(wide) == pastFloat);
            CHECK(ECSEngine::LengthSq(wide) == pastFloat * pastFloat);
            CHECK(ECSEngine::ooLength(wide) * pastFloat == doctest::Approx(1.0));
            CHECK(static_cast<float>(pastFloat) != pastFloat);

            const ECSEngine::dvec3 triple(3.0, 4.0, 0.0);
            CHECK(ECSEngine::Length(triple) == 5.0);
            CHECK(ECSEngine::Dot(triple, ECSEngine::dvec3(1.0, 1.0, 1.0)) == 7.0);

            const ECSEngine::dvec3 axis = ECSEngine::Normalize(ECSEngine::dvec3(0.0, 5.0, 0.0));
            CHECK(axis == ECSEngine::dvec3(0.0, 1.0, 0.0));

            const ECSEngine::dvec3 crossed = ECSEngine::Cross(ECSEngine::dvec3(1.0, 0.0, 0.0), ECSEngine::dvec3(0.0, 1.0, 0.0));
            CHECK(crossed == ECSEngine::dvec3(0.0, 0.0, 1.0));

            const ECSEngine::dvec3 absolute = ECSEngine::Abs(ECSEngine::dvec3(-pastFloat, 0.0, -4.0));
            CHECK(absolute.x == pastFloat);
            CHECK(absolute.z == 4.0);

            CHECK((8.0 - ECSEngine::dvec3(2.0, 4.0, 8.0)) == ECSEngine::dvec3(6.0, 4.0, 0.0));
        }
    }
}