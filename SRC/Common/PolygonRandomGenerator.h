#pragma once

namespace ECSEngine
{
struct RandomPolygonGenerationParameters
{
    u32 NumberOfPoints = 0;
};

class Polygon2D;
class Triangle2D;
class PolygonRandomGenerator
{
public:
    PolygonRandomGenerator() { }

    void GenerateRandomPoints(const Polygon2D& parPolygon, const RandomPolygonGenerationParameters& parGenerationParameters, std::vector<glm::vec2>& parOutRandomPoints);
    void GenerateRandomPoints(const Polygon2D& parPolygon,
          const std::vector<Polygon2D>& parHoles,
          RandomPolygonGenerationParameters& parGenerationParameters,
          std::vector<glm::vec2>& parOutRandomPoints);
    void GenerateRandomPoints(const std::vector<Triangle2D>& parPolygonTriangulation,
          const RandomPolygonGenerationParameters& parGenerationParameters,
          std::vector<glm::vec2>& parOutRandomPoints);

private:
    glm::vec2 GenerateOneRandomPoint(const std::vector<Triangle2D>& parPolygonTriangulation, const std::vector<std::pair<float, u32>>& parDistribution);
};
} // namespace ECSEngine
