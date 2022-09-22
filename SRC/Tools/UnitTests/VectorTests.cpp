#include "stdafx.h"
#include "doctest.h"
#include "Math/Vector.h"
#include "Math/VectorUtils.h"

template<typename VecType>
void TestVector2()
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

    // TODO dot
}


TEST_CASE("Vector2 CTORS tests")
{
    SUBCASE("Vec2 floats")
    {
        TestVector2<ECSEngine::vec2>();
    }

    SUBCASE("Vec2 int")
    {
        TestVector2<ECSEngine::ivec2>();
    }

    SUBCASE("Vec2 uint")
    {
        TestVector2<ECSEngine::uvec2>();
    }
}