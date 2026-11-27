#include "stdafx.h"

#include "Math/MathFunctions.h"
#include "Math/Matrix.h"
#include "Math/MatrixUtils.h"
#include "Math/Vector.h"
#include "doctest.h"

using namespace ECSEngine;

namespace
{
void CheckClose(float actual, float expected)
{
    CHECK(actual == doctest::Approx(expected).epsilon(1e-5f));
}

void CheckClose(const vec3& actual, const vec3& expected)
{
    CheckClose(actual.x, expected.x);
    CheckClose(actual.y, expected.y);
    CheckClose(actual.z, expected.z);
}

void CheckClose(const vec4& actual, const vec4& expected)
{
    CheckClose(actual.x, expected.x);
    CheckClose(actual.y, expected.y);
    CheckClose(actual.z, expected.z);
    CheckClose(actual.w, expected.w);
}

void CheckClose(const mat4& actual, const mat4& expected)
{
    for (u32 i = 0; i < 16; ++i)
        CheckClose(actual.FValues[i], expected.FValues[i]);
}
} // namespace

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

            const mat2 doubled = 2.f * t2;
            CHECK(doubled.FValues[1] == t2.FValues[1] * 2.f);
            CHECK(doubled.FValues[2] == t2.FValues[2] * 2.f);
        }

        SUBCASE("Multiply, transpose, determinant, inverse")
        {
            const mat2 shear(vec2(1.f, 0.f), vec2(1.f, 1.f));
            const mat2 shift(vec2(1.f, 1.f), vec2(0.f, 1.f));
            const mat2 product = shear * shift;
            CHECK(product.Column(0) == vec2(2.f, 1.f));
            CHECK(product.Column(1) == vec2(1.f, 1.f));
            CHECK(shear * vec2(1.f, 0.f) == shear.Column(0));

            const mat2 source(vec2(1.f, 2.f), vec2(3.f, 4.f));
            const mat2 transposed = Transpose(source);
            CHECK(transposed.Column(0) == source.Row(0));
            CHECK(transposed.Column(1) == source.Row(1));

            const mat2 linear(vec2(4.f, 1.f), vec2(2.f, 3.f));
            CHECK(Determinant(linear) == 10.f);
            const mat2 restored = linear * Invert(linear);
            CHECK(restored.FValues[0] == doctest::Approx(1.f));
            CHECK(restored.FValues[1] == doctest::Approx(0.f));
            CHECK(restored.FValues[2] == doctest::Approx(0.f));
            CHECK(restored.FValues[3] == doctest::Approx(1.f));

            CHECK((mat2::Identity() + mat2::Identity()).FValues[0] == 2.f);
            CHECK((mat2::Identity() - mat2::Identity()).FValues[0] == 0.f);
            CHECK((mat2::Identity() - mat2::Identity()).FValues[1] == 0.f);
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
            CHECK(t0.FValues[3] == 2.f);
            CHECK(t0.FValues[6] == 3.f);
            CHECK(t0.FValues[1] == 0.f);
            CHECK(t0.FValues[4] == 1.f);
            CHECK(t0.FValues[7] == 4.f);
            CHECK(t0.FValues[2] == 5.f);
            CHECK(t0.FValues[5] == 6.f);
            CHECK(t0.FValues[8] == 0.f);
        }

        SUBCASE("Inversion")
        {
            CHECK(t0_i.FValues[0] == -24.f);
            CHECK(t0_i.FValues[3] == 18.f);
            CHECK(t0_i.FValues[6] == 5.f);
            CHECK(t0_i.FValues[1] == 20.f);
            CHECK(t0_i.FValues[4] == -15.f);
            CHECK(t0_i.FValues[7] == -4.f);
            CHECK(t0_i.FValues[2] == -5.f);
            CHECK(t0_i.FValues[5] == 4.f);
            CHECK(t0_i.FValues[8] == 1.f);
        }

        SUBCASE("Multiplication")
        {
            CHECK(t1.FValues[0] == -24.f);
            CHECK(t1.FValues[3] == 18.f);
            CHECK(t1.FValues[6] == 5.f);
            CHECK(t1.FValues[1] == 20.f);
            CHECK(t1.FValues[4] == -15.f);
            CHECK(t1.FValues[7] == -4.f);
            CHECK(t1.FValues[2] == -5.f);
            CHECK(t1.FValues[5] == 4.f);
            CHECK(t1.FValues[8] == 1.f);
        }

        SUBCASE("Determinant, inverse product, transpose and vector")
        {
            CHECK(Determinant(t0) == 1.f);
            const mat3 product = t0 * t0_i;
            for (u32 i = 0; i < 9; ++i)
            {
                const float expected = (i == 0 || i == 4 || i == 8) ? 1.f : 0.f;
                CHECK(product.FValues[i] == doctest::Approx(expected));
            }

            CHECK(t0 * vec3(1.f, 0.f, 0.f) == t0.Column(0));

            const mat3 transposed = Transpose(t0);
            CHECK(transposed.Column(0) == t0.Row(0));
            CHECK(transposed.Column(1) == t0.Row(1));
            CHECK(transposed.Column(2) == t0.Row(2));
        }
    }

    TEST_CASE("Matrix4x4f")
    {
        SUBCASE("Identity and scalar")
        {
            const mat4 id = mat4::Identity();
            const vec4 point(1.f, 2.f, 3.f, 4.f);
            CheckClose(id * point, point);
            CheckClose(id * 2.f, 2.f * id);
        }

        SUBCASE("Translation")
        {
            const vec3 offset(3.f, -2.f, 5.f);
            const mat4 translated = Translation(offset);
            const vec4 point(1.f, 2.f, 3.f, 1.f);
            const vec4 direction(1.f, 2.f, 3.f, 0.f);

            CheckClose(GetTranslation(translated), offset);
            CheckClose(translated * point, vec4(4.f, 0.f, 8.f, 1.f));
            CheckClose(translated * direction, direction);
            CheckClose(Invert(translated) * (translated * point), point);

            const mat4 transposed = Transpose(translated);
            CHECK(transposed.Column(0) == translated.Row(0));
            CHECK(transposed.Column(3) == translated.Row(3));
            CheckClose(GetTranslation(GetRotationAsMat4(translated)), vec3(0.f));
            CheckClose(GetRotation(translated) * vec3(1.f, 2.f, 3.f), vec3(1.f, 2.f, 3.f));
        }

        SUBCASE("Rotation and euler")
        {
            const mat4 yaw = Rotation(Pi() * 0.5f, vec3(0.f, 0.f, 1.f));
            CheckClose(yaw * vec4(1.f, 0.f, 0.f, 0.f), vec4(0.f, 1.f, 0.f, 0.f));
            CheckClose(yaw * vec4(0.f, 1.f, 0.f, 0.f), vec4(-1.f, 0.f, 0.f, 0.f));

            CheckClose(EulerAnglesXYZ(0.f, 0.f, 0.f), mat4::Identity());
            const mat4 rolled = EulerAnglesXYZ(Pi() * 0.5f, 0.f, 0.f);
            CheckClose(rolled * vec4(1.f, 0.f, 0.f, 0.f), vec4(0.f, 1.f, 0.f, 0.f));
        }

        SUBCASE("Look at, perspective and orthographic")
        {
            const mat4 view = LookAt(vec3(0.f, 0.f, 5.f), vec3(0.f, 0.f, 6.f), vec3(0.f, 1.f, 0.f));
            CheckClose(view * vec4(0.f, 0.f, 6.f, 1.f), vec4(0.f, 0.f, 1.f, 1.f));
            CheckClose(view * vec4(1.f, 0.f, 5.f, 1.f), vec4(1.f, 0.f, 0.f, 1.f));

            const mat4 projected = Perspective(Pi() * 0.5f, 1.f, 1.f, 100.f);
            const vec4 onAxis = projected * vec4(0.f, 0.f, 10.f, 1.f);
            CheckClose(onAxis.x, 0.f);
            CheckClose(onAxis.y, 0.f);
            CheckClose(onAxis.w, 10.f);
            CheckClose((projected * vec4(1.f, 0.f, 10.f, 1.f)).x, 1.f);

            const mat4 ortho = Orthographic(0.f, 2.f, 0.f, 4.f);
            CheckClose(ortho * vec4(1.f, 2.f, 0.f, 1.f), vec4(0.f, 0.f, 0.f, 1.f));
        }
    }
}