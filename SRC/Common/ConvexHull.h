#pragma once

namespace ECSEngine
{
class Polygon2D;
void ConvexHullKeepOriginalArray(std::vector<glm::vec2>& parPoints, Polygon2D& outPolygon);
void ConvexHull(std::vector<glm::vec2>& parPoints, Polygon2D& outPolygon);
} // namespace ECSEngine