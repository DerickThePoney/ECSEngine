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

    void append(const std::vector<glm::vec2>& parPoints) { FPoints.insert(FPoints.end(), parPoints.begin(), parPoints.end()); }
    void push_back(const glm::vec2& parPoint) { FPoints.push_back(parPoint); }
    void reserve(const std::size_t parSize) { FPoints.reserve(parSize); }
    void resize(const std::size_t parSize) { FPoints.resize(parSize); }
    void resize(const std::size_t parSize, const glm::vec2& parDefault) { FPoints.resize(parSize, parDefault); }

    std::size_t size() const { return FPoints.size(); }
    bool empty() const { return FPoints.empty(); }

    void erase(const size_t at) { FPoints.erase(FPoints.begin() + at); }
    void erase(const std::vector<glm::vec2>::iterator at) { FPoints.erase(at); }
    void erase(const std::vector<glm::vec2>::const_iterator at) { FPoints.erase(at); }

    glm::vec2& operator[](const size_t at) { return FPoints[at]; }
    const glm::vec2 operator[](const size_t at) const { return FPoints[at]; }

    std::vector<glm::vec2>& data() { return FPoints; }
    const std::vector<glm::vec2>& data() const { return FPoints; }

    void set_points(const std::vector<glm::vec2>& parPoints) { FPoints = parPoints; }
    void set_points(std::vector<glm::vec2>&& parPoints) { FPoints = parPoints; }

    bool IsClockWise() const;
    Polygon2D Revert() const;

    template<class Archive>
    void serialize(Archive& ar)
    {
        PROPERTYFIELD(Points, std::vector<glm::vec2>({ glm::vec2(0.f, 0.f), glm::vec2(1.f, 0.f), glm::vec2(0.f, 1.f) }));
    }

private:
    std::vector<glm::vec2> FPoints;
};
} // namespace ECSEngine
