#pragma once

namespace ECSEngine
{
class Polygon2D;
class Triangle2D;
class PolygonPartionner
{
public:
    PolygonPartionner() { }

    // std::vector<Triangle2D> Triangulate(const Polygon2D& parPolygon);
    std::vector<Polygon2D> Partition(const Polygon2D& parPolygon, const std::vector<Polygon2D> parPolygonHoles);
    // std::vector<Triangle2D> Triangulate(const Polygon2D& parPolygon, const std::vector<Polygon2D> parPolygonHoles, Polygon2D& outExtentedPolygon);
};
} // namespace ECSEngine