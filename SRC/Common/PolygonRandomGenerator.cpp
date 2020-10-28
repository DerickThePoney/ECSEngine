#include "stdafx.h"

#include "PolygonRandomGenerator.h"

#include "IntersectionRoutines.h"
#include "Polygon.h"
#include "PolygonTriangulator.h"
#include "RandomGenerator.h"
#include "Triangle.h"

namespace ECSEngine
{

void PolygonRandomGenerator::GenerateRandomPoints(const Polygon2D& parPolygon,
      const RandomPolygonGenerationParameters& parGenerationParameters,
      std::vector<glm::vec2>& parOutRandomPoints)
{
    PolygonTriangulator polyTri;
    std::vector<Triangle2D> polygonTriangulation = polyTri.Triangulate(parPolygon);

    GenerateRandomPoints(polygonTriangulation, parGenerationParameters, parOutRandomPoints);
}

void PolygonRandomGenerator::GenerateRandomPoints(const Polygon2D& parPolygon,
      const std::vector<Polygon2D>& parHoles,
      RandomPolygonGenerationParameters& parGenerationParameters,
      std::vector<glm::vec2>& parOutRandomPoints)
{
    PolygonTriangulator polyTri;
    std::vector<Triangle2D> polygonTriangulation = polyTri.Triangulate(parPolygon, parHoles);

    GenerateRandomPoints(polygonTriangulation, parGenerationParameters, parOutRandomPoints);
}

void PolygonRandomGenerator::GenerateRandomPoints(const std::vector<Triangle2D>& parPolygonTriangulation,
      const RandomPolygonGenerationParameters& parGenerationParameters,
      std::vector<glm::vec2>& parOutRandomPoints)
{
    // 1- Sort triangles by area
    std::vector<std::pair<float, u32>> trianglesArea;
    trianglesArea.reserve(parPolygonTriangulation.size());
    forrange(i, 0, parPolygonTriangulation.size()) { trianglesArea.push_back({ parPolygonTriangulation[i].Area(), (u32)i }); }
    std::sort(trianglesArea.begin(), trianglesArea.end(), [](auto& a, auto& b) { return a.first > b.first; });

    float totalArea = 0.f;
    forrange(i, 0, trianglesArea.size()) { totalArea += trianglesArea[i].first; }

    float accum = 0.f;
    forrange(i, 0, trianglesArea.size())
    {
        float localArea = trianglesArea[i].first;
        trianglesArea[i].first = accum / totalArea;
        accum += localArea;
    }
    AlwaysCheckedAssert(accum == totalArea);

    forrange(i, 0, parGenerationParameters.NumberOfPoints) { parOutRandomPoints.push_back(GenerateOneRandomPoint(parPolygonTriangulation, trianglesArea)); }
}

glm::vec2 PolygonRandomGenerator::GenerateOneRandomPoint(const std::vector<Triangle2D>& parPolygonTriangulation, const std::vector<std::pair<float, u32>>& parDistribution)
{
    // 2- Generate a random float and pick the triangle
    const float randomNumber = RandomNumbers::NextFloat();
    u32 pickedTriangle = -1;
    forrange(i, 1, parDistribution.size())
    {
        if (parDistribution[i].first > randomNumber)
        {
            pickedTriangle = parDistribution[i - 1].second;
            break;
        }
    }
    if (pickedTriangle == -1)
        pickedTriangle = (u32)(parDistribution.size() - 1);

    // 3 - Generate 2 random floats and generate the point from the triangle
    const Triangle2D& triangle = parPolygonTriangulation[pickedTriangle];
    glm::vec2 randomPoint(0.0f);
    do
    {
        const float randomU = RandomNumbers::NextFloat();
        const float randomV = RandomNumbers::NextFloat();
        randomPoint = randomU * (triangle.B - triangle.A) + randomV * (triangle.C - triangle.A) + triangle.A;
    } while (!Intersection::PointInTriangle2D(triangle, randomPoint));

    return randomPoint;
}

} // namespace ECSEngine