#pragma once

namespace ECSEngine
{
class Polygon2D;
class Triangle2D;
class PolygonTriangulator
{
public:
    PolygonTriangulator() { }

    std::vector<Triangle2D> Triangulate(const Polygon2D& parPolygon);
};
} // namespace ECSEngine
