#include "stdafx.h"

#include "Common/BoundingBox.h"
#include "Common/IntersectionRoutines.h"
#include "Common/RandomGenerator.h"
#include "Math/Vector.h"
#include "doctest.h"

using namespace ECSEngine;

TEST_SUITE("Intersection tests")
{
    TEST_CASE("Sphere - AABB Intersection tests")
    {
        BoundingBox<vec3> aabbCentered(vec3(-1.f), vec3(1.f));
        BoundingBox<vec3> aabbNotCentered(vec3(-10.f), vec3(-8.f));

        vec4 sphereCentered(vec3(0.f), 0.5f);
        vec4 sphereNotCentered(vec3(-9.0f), 0.5f);

        SUBCASE("Sphere enclosed box")
        {
            {
                const bool intersectionTest = Intersection::SphereBoundingBoxIntersect(sphereCentered, aabbCentered);
                CHECK(intersectionTest);
            }

            {
                const bool intersectionTest = Intersection::SphereBoundingBoxIntersect(sphereNotCentered, aabbNotCentered);
                CHECK(intersectionTest);
            }
        }

        SUBCASE("Sphere outside box")
        {
            {
                const bool intersectionTest = Intersection::SphereBoundingBoxIntersect(sphereNotCentered, aabbCentered);
                CHECK(!intersectionTest);
            }

            {
                const bool intersectionTest = Intersection::SphereBoundingBoxIntersect(sphereCentered, aabbNotCentered);
                CHECK(!intersectionTest);
            }
        }

        SUBCASE("Sphere intersecting box")
        {
            {
                vec4 sphere(vec3(1.f), 0.5f);
                const bool intersectionTest = Intersection::SphereBoundingBoxIntersect(sphere, aabbCentered);
                CHECK(intersectionTest);
            }

            {
                vec4 sphere(vec3(-10.f), 0.5f);
                const bool intersectionTest = Intersection::SphereBoundingBoxIntersect(sphere, aabbNotCentered);
                CHECK(intersectionTest);
            }

            {
                vec4 sphere(vec3(1.5f, 0.f, 0.f), 0.5f);
                const bool intersectionTest = Intersection::SphereBoundingBoxIntersect(sphere, aabbCentered);
                CHECK(intersectionTest);
            }

            {
                vec4 sphere(vec3(-10.5f, -9.f, -9.f), 0.5f);
                const bool intersectionTest = Intersection::SphereBoundingBoxIntersect(sphere, aabbNotCentered);
                CHECK(intersectionTest);
            }
        }

        SUBCASE("Sphere containing box")
        {
            {
                sphereCentered.w = 5.f;
                const bool intersectionTest = Intersection::SphereBoundingBoxIntersect(sphereCentered, aabbCentered);
                sphereCentered.w = .5f;
                CHECK(intersectionTest);
            }

            {
                sphereNotCentered.w = 5.f;
                const bool intersectionTest = Intersection::SphereBoundingBoxIntersect(sphereNotCentered, aabbNotCentered);
                sphereNotCentered.w = .5f;
                CHECK(intersectionTest);
            }
        }
    }

    TEST_CASE("Ray - AABB Intersection tests")
    {
        BoundingBox<vec3> aabbCentered(vec3(-1.f), vec3(1.f));

        SUBCASE("Centered AABB Tests")
        {
            ECSEngine::RandomNumbers::InitRandomNumberGenerator(56);
            for (int i = 0; i < 1000; ++i)
            {
                vec3 Origin(ECSEngine::RandomNumbers::NextFloat(), ECSEngine::RandomNumbers::NextFloat(), ECSEngine::RandomNumbers::NextFloat());
                Ray3D ray(Origin, Normalize(vec3(0.f) - Origin));

                const bool intersectionTest = Intersection::RayAABBIntersection(ray, aabbCentered);

                CHECK(intersectionTest);
            }

            // TODO CHECK NON INTERSECTION TESTS

            ECSEngine::RandomNumbers::DestroyRandomNumberGenerator();
        }
    }
}