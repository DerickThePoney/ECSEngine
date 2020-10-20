#include "stdafx.h"

#include "RandomGenerator.h"

#include "Singleton.h"

#include <random>

namespace ECSEngine
{
class RandomFloatGenerator
{
public:
    RandomFloatGenerator(const u32 parSeed = 0);

    float NextFloat() { return FDistribution(FRandomEngine); }
    void SetSeed(const u32 parSeed)
    {
        FRandomEngine = std::mt19937(parSeed);
        FSeed = parSeed;
    }

private:
    std::mt19937 FRandomEngine;
    std::uniform_real_distribution<float> FDistribution;
    u32 FSeed;
};

RandomFloatGenerator::RandomFloatGenerator(const u32 parSeed /*= 0*/)
    : FDistribution(0.0f, 1.0f)
    , FSeed(parSeed)
{
    FRandomEngine.seed(parSeed);
}

class MainRandomGenerator final : public Singleton<MainRandomGenerator>
{
public:
    MainRandomGenerator()
        : Singleton()
    {
    }
    ~MainRandomGenerator() { }

    void SetFloatSeed(const u32 parSeed) { FRandomFloatGenerator.SetSeed(parSeed); }
    float NextFloat() { return FRandomFloatGenerator.NextFloat(); }

private:
    RandomFloatGenerator FRandomFloatGenerator;
};

namespace RandomNumbers
{

void InitRandomNumberGenerator(const u32 parSeed)
{
    AssertRelease(!MainRandomGenerator::HasInstance());
    MainRandomGenerator::CreateIFP();
    AssertRelease(MainRandomGenerator::HasInstance());

    MainRandomGenerator::Instance().SetFloatSeed(parSeed);
}

void DestroyRandomNumberGenerator()
{
    AssertRelease(MainRandomGenerator::HasInstance());
    MainRandomGenerator::Destroy();
    AssertRelease(!MainRandomGenerator::HasInstance());
}

float NextFloat()
{
    AssertRelease(MainRandomGenerator::HasInstance());
    return MainRandomGenerator::Instance().NextFloat();
}

} // namespace RandomNumbers

} // namespace ECSEngine