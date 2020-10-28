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
    std::vector<Triangle2D> Triangulate(const Polygon2D& parPolygon, const std::vector<Polygon2D> parPolygonHoles);
    std::vector<Triangle2D> Triangulate(const Polygon2D& parPolygon, const std::vector<Polygon2D> parPolygonHoles, Polygon2D& outExtentedPolygon);
};
} // namespace ECSEngine
