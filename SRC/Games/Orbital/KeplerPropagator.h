#pragma once

namespace ECSEngine
{

struct KeplerPropagatorState
{
    double FMu = 0.0;
    double FEccentricity = 0.0;
    double FSemiMajor = 0.0;
    double FMeanMotion = 0.0;
    double FHMag = 0.0;
    double FMeanAnomaly0 = 0.0;
    double FOrbitalEnergy = 0.0;

    dvec3 FPeriapsisDirection;
    dvec3 FMotionDirection;

    double FElapsed = 0.0;
    bool FbActive = false;
};

struct KeplerPropagator
{
public:
    static bool TryCapture(KeplerPropagatorState& OutState, const dvec3& PlanetPos, const dvec3& PlanetVel, const dvec3& RocketPos, const dvec3& RocketVel, const double Mu);

    // Relative to the parent at the evaluation time. Caller does:
    //   vesselPos = parentPos + outR;
    //   vesselVel = parentVel + outV;
    static bool Advance(KeplerPropagatorState& InOutState, double Dt, dvec3& OutRelativePos, dvec3& OutRelativeVel);
};
} // namespace ECSEngine