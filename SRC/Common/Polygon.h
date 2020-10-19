#pragma once

namespace ECSEngine
{
class Polygon2D
{
public:
    Polygon2D() { }

    std::vector<glm::vec2>::iterator begin() { return FPoints.begin(); }
    std::vector<glm::vec2>::iterator end() { return FPoints.end(); }
    std::vector<glm::vec2>::const_iterator cbegin() const { return FPoints.cbegin(); }
    std::vector<glm::vec2>::const_iterator cend() const { return FPoints.cend(); }

    std::vector<glm::vec2>::reverse_iterator rbegin() { return FPoints.rbegin(); }
    std::vector<glm::vec2>::reverse_iterator rend() { return FPoints.rend(); }
    std::vector<glm::vec2>::const_reverse_iterator crbegin() const { return FPoints.crbegin(); }
    std::vector<glm::vec2>::const_reverse_iterator crend() const { return FPoints.crend(); }

    void push_back(const glm::vec2& parPoint) { FPoints.push_back(parPoint); }

    std::size_t size() const { return FPoints.size(); }

    glm::vec2& operator[](const size_t at) { return FPoints[at]; }
    const glm::vec2 operator[](const size_t at) const { return FPoints[at]; }

    bool IsClockWise() const;
    Polygon2D Revert() const;

private:
    std::vector<glm::vec2> FPoints;
};
} // namespace ECSEngine
