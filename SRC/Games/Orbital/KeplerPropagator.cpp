#include "stdafx.h"

#include "KeplerPropagator.h"

namespace ECSEngine
{

bool KeplerPropagator::TryCapture(KeplerPropagatorState& OutState, const dvec3& PlanetPos, const dvec3& PlanetVel, const dvec3& RocketPos, const dvec3& RocketVel, const double Mu)
{
    OutState.FbActive = false;

    if (Mu <= 0)
        return false;

    const dvec3 r = RocketPos - PlanetPos;
    const dvec3 v = RocketVel - PlanetVel;

    const double rMag = Length(r);
    if (rMag < 1e-3)
        return false;

    OutState.FMu = Mu;
    OutState.FOrbitalEnergy = 0.5 * LengthSq(v) - Mu / rMag;
    if (abs(OutState.FOrbitalEnergy) < 1e-6)
        return false;

    const dvec3 h = Cross(r, v);
    const double hSq = LengthSq(h);
    if (hSq < 1e-4)
        return false;

    OutState.FHMag = sqrt(hSq);

    const dvec3 eVec = Cross(v, h) / Mu - r / rMag;
    OutState.FEccentricity = Length(eVec);
    if (OutState.FOrbitalEnergy < 0 && OutState.FEccentricity >= 1.0)
        return false;

    OutState.FSemiMajor = -Mu / (2 * OutState.FOrbitalEnergy);

    if (OutState.FOrbitalEnergy > 0 && (OutState.FEccentricity <= 1.0 || OutState.FSemiMajor >= 0))
        return false;

    // Perifocal basis
    if ((OutState.FEccentricity * OutState.FEccentricity) < 1.e-8)
    {
        const dvec3 fallback = (fabs(h.y) < 0.9f * OutState.FHMag) ? dvec3(0.f, 1.f, 0.f) : dvec3(1.f, 0.f, 0.f);
        OutState.FPeriapsisDirection = Normalize(Cross(h, fallback));
    }
    else
    {
        OutState.FPeriapsisDirection = Normalize(eVec);
    }

    OutState.FMotionDirection = Normalize(Cross(h, OutState.FPeriapsisDirection));

    // True anomaly at capture
    double cosv = Clamp(Dot(r, OutState.FPeriapsisDirection) / rMag, -1.0, 1.0);
    double sinv = Dot(r, OutState.FMotionDirection) / rMag;

    // Ellipse case
    if (OutState.FOrbitalEnergy < 0)
    {
        const double a = OutState.FSemiMajor;
        OutState.FMeanMotion = sqrt(Mu / (a * a * a));
        double cosE = Clamp((OutState.FEccentricity + cosv) / (1 + OutState.FEccentricity * cosv), -1.0, 1.0);
        double sinE = sqrt(1 - OutState.FEccentricity * OutState.FEccentricity) * sinv / (1 + OutState.FEccentricity * cosv);

        double E = atan2(sinE, cosE);
        OutState.FMeanAnomaly0 = E - OutState.FEccentricity * Sin(E);
    }
    else
    {
        // Hyperbola case (a < 0): use a*a*a, not pow(a, 3) — pow(negative, 3.0) is NaN
        const double a = OutState.FSemiMajor;
        OutState.FMeanMotion = sqrt(-Mu / (a * a * a));
        double coshH = (OutState.FEccentricity + cosv) / (1 + OutState.FEccentricity * cosv);
        double H = acosh(coshH);
        if (sinv < 0.0)
            H = -H;

        OutState.FMeanAnomaly0 = OutState.FEccentricity * sinh(H) - H;
    }

    OutState.FbActive = true;
    OutState.FElapsed = 0.0;

    return true;
}

bool KeplerPropagator::Advance(KeplerPropagatorState& InOutState, double Dt, dvec3& OutRelativePos, dvec3& OutRelativeVel)
{
    auto fail = [&]() -> bool
    {
        InOutState.FbActive = false;
        OutRelativePos = dvec3(0.0);
        OutRelativeVel = dvec3(0.0);
        return false;
    };

    if (!InOutState.FbActive)
    {
        AssertNotReached();
        OutRelativePos = dvec3(0.0);
        OutRelativeVel = dvec3(0.0);
        return false;
    }

    const double hMag = InOutState.FHMag;
    if (hMag < 1e-12)
        return fail();

    InOutState.FElapsed += Dt;

    const double e = InOutState.FEccentricity;
    const double a = InOutState.FSemiMajor;
    const double n = InOutState.FMeanMotion;
    const double mu = InOutState.FMu;
    const dvec3& periDir = InOutState.FPeriapsisDirection;
    const dvec3& motionDir = InOutState.FMotionDirection;

    constexpr double DenomFloor = 1e-12;
    constexpr double ResidualTol = 1e-8;

    double cosNu = 0.0;
    double sinNu = 0.0;
    double rho = 0.0;

    if (InOutState.FOrbitalEnergy < 0.0)
    {
        // Ellipse: M = E - e sin E
        double M = InOutState.FMeanAnomaly0 + n * InOutState.FElapsed;
        const double twoPi = 2.0 * Pi_dbl();
        M = fmod(M + Pi_dbl(), twoPi);
        if (M < 0.0)
            M += twoPi;
        M -= Pi_dbl(); // (-pi, pi]

        double E = M + e * Sin(M);
        for (int i = 0; i < 8; ++i)
        {
            const double f = E - e * Sin(E) - M;
            const double df = 1.0 - e * Cos(E);
            if (abs(df) < DenomFloor)
                return fail();
            E -= f / df;
        }

        if (abs(E - e * Sin(E) - M) > ResidualTol)
            return fail();

        const double cosE = Cos(E);
        const double sinE = Sin(E);
        const double oneMinusECosE = 1.0 - e * cosE;
        if (abs(oneMinusECosE) < DenomFloor)
            return fail();

        cosNu = Clamp((cosE - e) / oneMinusECosE, -1.0, 1.0);
        sinNu = sqrt(Max(0.0, 1.0 - e * e)) * sinE / oneMinusECosE;
        rho = a * oneMinusECosE;
    }
    else
    {
        // Hyperbola: N = e sinh H - H
        const double N = InOutState.FMeanAnomaly0 + n * InOutState.FElapsed;

        double H = asinh(N / Max(e, 1e-6));
        for (int i = 0; i < 8; ++i)
        {
            const double f = e * sinh(H) - H - N;
            const double df = e * cosh(H) - 1.0;
            if (abs(df) < DenomFloor)
                return fail();
            H -= f / df;
        }

        if (abs(e * sinh(H) - H - N) > ResidualTol)
            return fail();

        const double coshH = cosh(H);
        const double sinhH = sinh(H);
        const double eCoshHMinus1 = e * coshH - 1.0;
        if (abs(eCoshHMinus1) < DenomFloor)
            return fail();

        // cos ν = (e - cosh H) / (e cosh H - 1)  (not (cosh H - e)/...)
        cosNu = (e - coshH) / eCoshHMinus1;
        sinNu = sqrt(Max(0.0, e * e - 1.0)) * sinhH / eCoshHMinus1;
        rho = a * (1.0 - e * coshH); // a < 0, rho > 0
    }

    const dvec3 radialDir = periDir * cosNu + motionDir * sinNu;
    const dvec3 tangentDir = periDir * (-sinNu) + motionDir * cosNu;

    const double vr = (mu / hMag) * e * sinNu;
    const double vt = (mu / hMag) * (1.0 + e * cosNu);

    OutRelativePos = radialDir * rho;
    OutRelativeVel = radialDir * vr + tangentDir * vt;
    return true;
}

} // namespace ECSEngine