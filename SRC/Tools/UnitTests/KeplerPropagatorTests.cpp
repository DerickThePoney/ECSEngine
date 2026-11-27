#include "stdafx.h"

#include "KeplerPropagator.h"
#include "Math/MathFunctions.h"
#include "Math/Vector.h"
#include "doctest.h"

using namespace ECSEngine;

namespace
{
constexpr double kMu = 8.0e6;
const dvec3 kOrigin(0.0);
const dvec3 kRest(0.0);

bool CaptureRelative(KeplerPropagatorState& OutState, const dvec3& r, const dvec3& v, double Mu = kMu)
{
    return KeplerPropagator::TryCapture(OutState, kOrigin, kRest, r, v, Mu);
}

double CircSpeed(double Radius, double Mu = kMu)
{
    return sqrt(Mu / Radius);
}
} // namespace

TEST_SUITE("KeplerPropagator")
{
    TEST_CASE("Ellipse epoch round-trip")
    {
        const double radius = 1600.0;
        const dvec3 r(radius, 0.0, 0.0);
        const dvec3 v(0.0, 0.0, CircSpeed(radius));

        KeplerPropagatorState state;
        REQUIRE(CaptureRelative(state, r, v));
        CHECK(state.FbActive);
        CHECK(state.FOrbitalEnergy < 0.0);
        CHECK(state.FEccentricity < 0.05);

        dvec3 outR, outV;
        REQUIRE(KeplerPropagator::Advance(state, 0.0, outR, outV));
        CHECK(Length(outR - r) < 1e-6 * radius);
        CHECK(Length(outV - v) < 1e-6 * CircSpeed(radius));
    }

    TEST_CASE("Ellipse one period returns to start")
    {
        const double radius = 1600.0;
        const dvec3 r(radius, 0.0, 0.0);
        const dvec3 v(0.0, 0.0, CircSpeed(radius));

        KeplerPropagatorState state;
        REQUIRE(CaptureRelative(state, r, v));

        const double a = state.FSemiMajor;
        const double period = 2.0 * Pi_dbl() * sqrt((a * a * a) / kMu);

        dvec3 outR, outV;
        REQUIRE(KeplerPropagator::Advance(state, period, outR, outV));
        CHECK(Length(outR - r) < 1e-3);
        CHECK(Length(outV - v) < 1e-3);
    }

    TEST_CASE("Near-circular ellipse period closes")
    {
        const double radius = 1600.0;
        // Slightly sub-circular speed keeps e tiny but non-zero.
        const dvec3 r(radius, 0.0, 0.0);
        const dvec3 v(0.0, 0.0, CircSpeed(radius) * 0.999);

        KeplerPropagatorState state;
        REQUIRE(CaptureRelative(state, r, v));
        CHECK(state.FEccentricity < 0.01);
        CHECK(state.FOrbitalEnergy < 0.0);

        const double a = state.FSemiMajor;
        const double period = 2.0 * Pi_dbl() * sqrt((a * a * a) / kMu);

        dvec3 outR, outV;
        REQUIRE(KeplerPropagator::Advance(state, period, outR, outV));
        CHECK(Length(outR - r) < 1e-2);
    }

    TEST_CASE("Hyperbola epoch round-trip")
    {
        // Periapsis-like flyby: r = 1000, v excess of circular so energy > 0.
        const dvec3 r(1000.0, 0.0, 0.0);
        const dvec3 v(0.0, 0.0, 150.0);

        KeplerPropagatorState state;
        REQUIRE(CaptureRelative(state, r, v));
        CHECK(state.FbActive);
        CHECK(state.FOrbitalEnergy > 0.0);
        CHECK(state.FEccentricity > 1.0);
        CHECK(state.FSemiMajor < 0.0);

        dvec3 outR, outV;
        REQUIRE(KeplerPropagator::Advance(state, 0.0, outR, outV));
        CHECK(Length(outR - r) < 1e-6 * Length(r));
        CHECK(Length(outV - v) < 1e-6 * Length(v));
    }

    TEST_CASE("Hyperbola flyby step stays active and moves")
    {
        const dvec3 r(1000.0, 0.0, 0.0);
        const dvec3 v(0.0, 0.0, 150.0);

        KeplerPropagatorState state;
        REQUIRE(CaptureRelative(state, r, v));

        dvec3 outR, outV;
        REQUIRE(KeplerPropagator::Advance(state, 5.0, outR, outV));
        CHECK(state.FbActive);
        CHECK(Length(outR - r) > 1.0);
        // Outside periapsis on an outbound leg, radius grows.
        CHECK(Length(outR) > Length(r) - 1e-6);
    }

    TEST_CASE("TryCapture rejects invalid inputs")
    {
        KeplerPropagatorState state;
        const dvec3 r(1600.0, 0.0, 0.0);
        const dvec3 vCirc(0.0, 0.0, CircSpeed(1600.0));

        SUBCASE("mu <= 0")
        {
            CHECK_FALSE(CaptureRelative(state, r, vCirc, 0.0));
            CHECK_FALSE(state.FbActive);
        }

        SUBCASE("near-zero r")
        {
            CHECK_FALSE(CaptureRelative(state, dvec3(1e-4, 0.0, 0.0), vCirc));
            CHECK_FALSE(state.FbActive);
        }

        SUBCASE("radial fall h ~ 0")
        {
            const dvec3 vRadial(-10.0, 0.0, 0.0);
            CHECK_FALSE(CaptureRelative(state, r, vRadial));
            CHECK_FALSE(state.FbActive);
        }

        SUBCASE("near-parabola")
        {
            // Escape speed at r: sqrt(2 mu / r). Energy ~ 0.
            const double vEsc = sqrt(2.0 * kMu / Length(r));
            const dvec3 vPar(0.0, 0.0, vEsc);
            CHECK_FALSE(CaptureRelative(state, r, vPar));
            CHECK_FALSE(state.FbActive);
        }
    }
}
